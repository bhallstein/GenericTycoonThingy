#include "Behaviour.hpp"
#include "Unit.hpp"
#include "Building.hpp"
#include "Furnishing.hpp"
#include "Level.hpp"
#include "LuaHelper.hpp"
#include "MrPaths.hpp"

Behaviour::Behaviour(const char *_type, W::EventHandler *_eh) : type(_type), eh(_eh)
{
	if (strcmp(_type, "despawn") == 0)     b = new DespawnBehaviour(eh);
	else if (strstarts(_type, "seek:"))    b = new SeekBehaviour(_type, eh);
	else if (strstarts(_type, "service:")) b = new ServiceBehaviour(_type, eh);
	else throw W::Exception("Behaviour with invalid type created");
}
Behaviour::~Behaviour()
{
	std::cout << "Behaviour \"" << type << "\" destruct" << std::endl;
	delete b;
}
bool Behaviour::initialize() {
	return BehaviourBase::initialize();
}
void Behaviour::update()    { b->update(); }
bool Behaviour::destroyed() { return b->destroyed; }
void Behaviour::destroy()   { b->destroyed = true; }
/* Initialiser passthroughs */
void Behaviour::init(Unit *u) { b->init(u); }
void Behaviour::init(Level *l, Unit *u) { b->init(l, u); } 
void Behaviour::init(Unit *u1, Unit *u2, Furnishing *f) { b->init(u1, u2, f); }
void Behaviour::init(Level *l, Unit *u1, Unit *u2, Furnishing *f) { b->init(l, u1, u2, f); }


/* BehaviourBase */

bool BehaviourBase::lua_initialized = false;
LuaHelper *BehaviourBase::mrLua;
bool BehaviourBase::initialize() {
	if (BehaviourBase::lua_initialized) return true;
	
	W::log << "  BehaviourBase::initialize() called..." << std::endl;
	mrLua = new LuaHelper();
	
	std::string path = MrPaths::luaPath;
	path.append("behaviours.lua");
	if (!mrLua->loadFile(path.c_str())) {
		W::log << "Could not read behaviours.lua" << std::endl;
		return false;
	}
	
	BehaviourBase::lua_initialized = true;
	
	return SeekBehaviour::initialize() && ServiceBehaviour::initialize();
}

/* DespawnBehaviour */

void DespawnBehaviour::init(Unit *_u) {
	unit = _u;
	initialized = true;
}
void DespawnBehaviour::update() {
	if (stage == -1) {		// Waiting
		if (++frames_waited >= WAIT_PERIOD) stage++;
	}
	else if (stage == 0) {	// Send unit to despawn location
		int x, y;
		unit->getDespawnPoint(&x, &y);
		if (unit->voyage(x, y)) stage++;
		else wait();
	}
	else if (stage == 1) {	// Despawn on arrival
		if (unit->arrived)
			unit->destroy(), destroyed = true;
	}
}


/* SeekBehaviour */

bool SeekBehaviour::lua_initialized = false;
std::map<std::string, struct seekBehaviourInfo> SeekBehaviour::types;

SeekBehaviour::SeekBehaviour(const char *_type, W::EventHandler *_eh) :
	BehaviourBase(_eh), type(_type)
{
	bType = &types[type];
}
void SeekBehaviour::init(Level *_l, Unit *_u) {
	level = _l, unit = _u;
	unit->capture();
	building = level->randomBuildingWithType(bType->requisiteBuildingType.c_str());
	initialized = true;
}
void SeekBehaviour::update() {
//	if (stage == 0) {		// Send unit to requisite establishment
//		int x, y;
//		building->getEntryPoint(&x, &y);
//		if (unit->voyage(x, y)) stage++;
//		else wait();
//	}
//	else if (stage == 1) {	// Send unit to queue point
	if (stage == 0) {
		int x, y;
		building->getQueuePoint(&x, &y);
		if (unit->voyage(x, y)) stage++;
		else wait();
	}
	else if (stage == 2) {	// Done!
		if (unit->arrived) {
			unit->nextBehaviour = bType->followingBehaviour.c_str();
			unit->release();
			building->addToQueue(unit);
			destroyed = true;
		}
	}
}
bool SeekBehaviour::initialize() {
	lua_State *L = mrLua->LuaInstance;
	
	// Construct types map
	if (!mrLua->pushtable("seekBehaviours")) {
		W::log << "In behaviours.lua, could not push the seekBehaviours table onto the stack" << std::endl;
		return false;
	}
	lua_pushnil(L);							// S: -1 nil; -2 seekBehaviours
	while (lua_next(L, -2)) {				// S: -1 behaviour; -2 key; -3 seekBehaviours
		if (!lua_istable(L, -1)) {		// If not a subtable, skip
			lua_pop(L, 1);
			continue;
		}
		const char *bType = lua_tostring(L, -2);
		struct seekBehaviourInfo *bInfo = &SeekBehaviour::types[bType];
		
		try {
			bInfo->requisiteBuildingType = mrLua->getfield<const char *>("requisiteBuildingType");
			bInfo->followingBehaviour = mrLua->getfield<const char *>("followingBehaviour");
		}
		catch (W::Exception &exc) {
			W::log << "Invalid SeekBehaviour definition in behaviours.lua for '" << bType << "' type" << std::endl;
			return false;
		}
		
		lua_pop(L, 1);						// S: -1 key; -2 buildingTypes
	}										// S: -1 buildingTypes
	lua_pop(L, 1);							// S: ~
	
	SeekBehaviour::lua_initialized = true;
	return true;
}


/* ServiceBehaviour */

bool ServiceBehaviour::lua_initialized = false;
std::map<std::string, serviceBehaviourInfo> ServiceBehaviour::types;

ServiceBehaviour::ServiceBehaviour(const char *_type, W::EventHandler *_eh) :
	BehaviourBase(_eh), type(_type)
{
	bType = &types[type];
}
ServiceBehaviour::~ServiceBehaviour()
{
	eh->unsubscribeFromEventType(W::EventType::INTERRUPT_UNITPICKUP, this);
}
void ServiceBehaviour::init(Level *_l, Unit *_u, Unit *_s, Furnishing *_f) {
	level = _l, unit = _u, staff = _s, furnishing = _f;
	// Check furnishing is capable of this interaction
	if (!furnishing->capableOfBehaviour(type.c_str())) {
		char s[150]; sprintf(s, "'%s' behaviour initialized with incompatible '%s' furnishing", type.c_str(), furnishing->type.c_str());
		throw W::Exception(s);
	}
	unit->capture(), staff->capture(), furnishing->capture();
	initialized = true;
	
	eh->subscribeToEventType(
		W::EventType::INTERRUPT_UNITPICKUP,
		new W::Callback(&ServiceBehaviour::receiveEvent, this)
	);
	
	//take the nice patron's money
	level->payPlayer(bType->charge);
}
void ServiceBehaviour::update() {
	if (stage == 0) {		// Send units to interaction points
		int uDestX, uDestY, sDestX, sDestY;
		furnishing->getInteractionPoint("civilian", &uDestX, &uDestY);
		furnishing->getInteractionPoint("staff", &sDestX, &sDestY);
		if (uDestX < 0 || sDestY < 0) { }	// Incompatible unit! What to do? Hmm.
		if (unit->voyage(uDestX, uDestY) && staff->voyage(sDestX, sDestY)) stage++;
		else wait();
	}
	else if (stage == 1) {	// Run animations
		if (unit->arrived && staff->arrived)
			unit->runAnimation(), staff->runAnimation(), stage++;
	}
	else if (stage == 2) {	// Release things
		if (unit->animation_finished && staff->animation_finished) {
			level->createBehaviour("despawn")->init(unit);
			unit->release(), staff->release(), furnishing->release();
			destroyed = true;
		}
	}
	
	// Interrupt handling
	else if (stage == 10) {	// Send unit to queue point
		int x, y;
		char s[120]; sprintf(s, "interrupt: getting queue point from building %p", contextBuilding);
		std::cout << s << std::endl;
		contextBuilding->getQueuePoint(&x, &y);
		if (unit->voyage(x, y)) stage++;
		else wait();
	}
	else if (stage == 11) {	// Done!
		if (unit->arrived) {
			unit->nextBehaviour = type;
			unit->release();
			contextBuilding->addToQueue(unit);
			destroyed = true;
		}
	}
}
void ServiceBehaviour::receiveEvent(W::Event *ev) {
	if (ev->type == W::EventType::INTERRUPT_UNITPICKUP) {
		if (ev->_payload != staff)
			return;
		// Interrupt the behaviour
		contextBuilding = staff->getContextBuilding();
		staff->release(), furnishing->release();
		stage = 10;
	}
}
bool ServiceBehaviour::initialize() {
	lua_State *L = mrLua->LuaInstance;
	
	// Construct types map
	if (!mrLua->pushtable("serviceBehaviours")) {
		W::log << "In behaviours.lua, could not push the serviceBehaviours table onto the stack" << std::endl;
		return false;
	}
	lua_pushnil(L);							// S: -1 nil; -2 serviceBehaviours
	while (lua_next(L, -2)) {				// S: -1 behaviour; -2 key; -3 serviceBehaviours
		if (!lua_istable(L, -1)) {		// If not a subtable, skip
			lua_pop(L, 1);
			continue;
		}
		const char *bType = lua_tostring(L, -2);
		struct serviceBehaviourInfo *bInfo = &ServiceBehaviour::types[bType];
		
		try {
			bInfo->charge = mrLua->getfield<int>("charge");
		}
		catch (W::Exception &exc) {
			W::log << "Invalid ServiceBehaviour definition in behaviours.lua for '" << bType << "' type" << std::endl;
			return false;
		}
		
		lua_pop(L, 1);						// S: -1 key; -2 serviceBehaviours
	}										// S: -1 serviceBehaviours
	lua_pop(L, 1);							// S: ~
	
	ServiceBehaviour::lua_initialized = true;
	return true;
}
