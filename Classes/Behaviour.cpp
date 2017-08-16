#include "Behaviour.hpp"
#include "Unit.hpp"
#include "Building.hpp"
#include "Furnishing.hpp"
#include "Level.hpp"
#include "LuaHelper.hpp"
#include "MrPaths.hpp"
#include "MrKlangy.hpp"
#include "types.hpp"


/*************/
/* Behaviour */
/*************/

bool Behaviour::lua_initialized = false;
LuaHelper *Behaviour::mrLua = new LuaHelper();

Behaviour::Behaviour() :
	stage(0), waiting(false)
{
	// hai bhvr
}
Behaviour::~Behaviour()
{
	// bai bhvr
}
bool Behaviour::initialize() {
	if (Behaviour::lua_initialized) return true;
	W::log << "  Behaviour::initialize()..." << std::endl;
	
	std::string path = MrPaths::luaPath;
	path.append("behaviours.lua");
	if (!mrLua->loadFile(path.c_str())) {
		W::log << "Could not read behaviours.lua" << std::endl;
		return false;
	}
	
	Behaviour::lua_initialized = true;
	return true; //SeekBehaviour::initialize() && ServiceBehaviour::initialize();
}


/******************************/
/* SuperBehaviour : Behaviour */
/******************************/

SuperBehaviour::SuperBehaviour() :
	yield_behaviour(NULL), suspended(false)
{
	// it r bird? it r plain? no, it r suprbhvr.
}
SuperBehaviour::~SuperBehaviour()
{
	// until next time
}
void SuperBehaviour::_update() {
	if (suspended) return;
	if (yield_behaviour) {
		yield_behaviour->update();
		if (yield_behaviour->destroyed)
			unyield();
	}
	else _superupdate();
}
void SuperBehaviour::yield(Behaviour *_b) {
	yield_behaviour = _b;
}
void SuperBehaviour::unyield() {
	if (!yield_behaviour)
		throw W::Exception("SuperBehaviour: unyield called when no yield behaviour set");
	delete yield_behaviour;
	yield_behaviour = NULL;
	stage++;
}
void SuperBehaviour::suspend() {
	suspended = true;
}
void SuperBehaviour::unsuspend() {
	suspended = false;
	stage++;
}


/************************************/
/* DispatchingBehaviour : Behaviour */
/************************************/

DispatchingBehaviour::DispatchingBehaviour() :
	sb(NULL), available(true)
{
	// hai dispatchy
}
bool DispatchingBehaviour::isAvailable() { return available; }


/**************************************/
/* CustomerBehaviour : SuperBehaviour */
/**************************************/

CustomerBehaviour::CustomerBehaviour(Unit *_u, Level *_l) :
	customer(_u), level(_l)
{
	printf("CustomerBehaviour %p created on unit %p\n", this, customer);
}
CustomerBehaviour::~CustomerBehaviour()
{
	// bai
}
void CustomerBehaviour::_superupdate() {
	if (stage == 0) {
		// Send to a shop
		destination_building = level->randomBuildingWithType("shop");
		if (!destination_building) {
			// No shop exists
			wait();
		}
		else {
			W::position dest;
			destination_building->getQueuePoint(dest);
			yield(new RouteBehaviour(customer, dest));
		}
	}
	else if (stage == 1) {
		// Queue
		// ...
	}
	else if (stage == 2) {
		// Browse
		// - to involve running TryoutBehaviours on the various items for sale in the shop
		// - also the decision as to what item to buy
		destination_building->customerEntering();
		itemForPurchase = destination_building->getRandomAvailableFurnishing();
		if (itemForPurchase) stage++;
		else throw W::Exception("No item available for purchase");
	}
	else if (stage == 3) {
		// Purchase
		if (ShopKeeperBehaviour *skb = destination_building->getRandomAvailableShopKeeperBehaviour()) {
			skb->dispatchUnit(customer, itemForPurchase, this);
			suspend();
		}
		else {
			// No shop keeper available
			wait();
		}
	}
	else if (stage == 5) {
		// Go home
		destination_building->customerLeaving();
		W::position dest;
		customer->getDespawnPoint(dest);
		yield(new RouteBehaviour(customer, dest));
	}
	else if (stage == 6) {
		// Despawn
		customer->destroy();
		destroy();
	}
}
void CustomerBehaviour::buildingAccessGranted() {
	if (stage != 1) throw W::Exception("Something odd has happened...");
	stage++;
}


/**********************************************/
/* ShopKeeperBehaviour : DispatchingBehaviour */
/**********************************************/

ShopKeeperBehaviour::ShopKeeperBehaviour(Unit *_sk, Level *_level) :
	sk(_sk), contextBuilding(NULL), level(_level)
{
	printf("ShopKeeperBehaviour %p created on unit %p\n", this, sk);
	sk->skBehaviour = this;
}
ShopKeeperBehaviour::~ShopKeeperBehaviour()
{
	printf("ShopKeeperBehaviour %p destruct\n", this);
}
void ShopKeeperBehaviour::_superupdate() {
	if (stage == 0) {
		// Do nothing
	}
	
	/**** Intra-shop behaviours ****/
	else if (stage == 100) {
		
	}
	
	/**** Extra-shop behaviours ****/
	else if (stage == 200) {
		
	}
	
	/**** Customer dispatch ****/
	else if (stage == 300) {
		yield(new PurchaseBehaviour(customer, sk, furnishing, level));
	}
	else if (stage == 301) {
		finishedDispatch();
	}
}
bool ShopKeeperBehaviour::dispatchUnit(Unit *_c, Furnishing *_f, SuperBehaviour *_sb) {
	if (!available) throw W::Exception("ShopKeeperBehaviour dispatch initiated, but SKB is unavailable");
	sb = _sb, customer = _c, furnishing = _f;
	available = false;
	stage = 300;	// Run dispatch behaviours
	return true;
}
void ShopKeeperBehaviour::finishedDispatch() {
	sb->unsuspend();
	available = true;
	stage = 100;	// Run intra-shop behaviours
	
	sb = NULL;
	customer = NULL;
	furnishing = NULL;
}

void ShopKeeperBehaviour::unitWasPickedUp() {
	std::cout << "sk picked up" << std::endl;
	if (contextBuilding) {
		std::cout << "removing from context building" << std::endl;
		contextBuilding->removeShopKeeper(this);
		contextBuilding = NULL;
	}
	suspend();
}
void ShopKeeperBehaviour::unitWasPutDown() {
	std::cout << "sk put down" << std::endl;
	unsuspend();
	if (Building *b = level->buildingAtPosition(sk->pos)) {
		std::cout << "adding to context building" << std::endl;
		contextBuilding = b;
		b->addShopKeeper(this);
		stage = 100;	// Run intra-shop behaviours
	}
	else {
		stage = 200;	// Run extra-shop behaviours
	}
}


/******************************/
/* RouteBehaviour : Behaviour */
/******************************/

RouteBehaviour::RouteBehaviour(Unit *_u, W::position &_dest) :
	unit(_u), dest(_dest)
{
	printf("RouteBehaviour %p created on unit %p\n", this, unit);
	unit->capture();
}
RouteBehaviour::~RouteBehaviour()
{
	printf("RouteBehaviour %p destruct\n", this);
}
void RouteBehaviour::_update() {
	if (stage == 0) {
		if (unit->voyage(dest)) stage++;
		else wait();
	}
	else if (stage == 1) {
		if (unit->arrived) {
			unit->release();
			destroy();
		}
	}
}


/******************************/
/* QueueBehaviour : Behaviour */
/******************************/

//QueueBehaviour::QueueBehaviour(W::EventHandler *_eh, Building *_b, int _qsize) :
//	DispatchingBehaviour("queue", _eh),
//	shop(_b), qsize(_qsize)
//{
//	// hey 
//}
//QueueBehaviour::~QueueBehaviour()
//{
//	
//}
//void QueueBehaviour::_update() {
//	if (!shop->customerCapacityReached())
//		dequeue();
//}
//bool QueueBehaviour::dispatchUnit(Unit *_u, SuperBehaviour *_sb) {
//	return queue(_u, _sb);
//}
//bool QueueBehaviour::queue(Unit *_u, SuperBehaviour *_sb) {
//	if (Q.size() >= qsize)
//		return false;
//	Q.push(_u);
//	sbs[_u] = _sb;
//	return true;
//}
//void QueueBehaviour::dequeue() {
//	if (Q.size()) {
//		Unit *u = Q.front();
//		Q.pop();
//		sbs[u]->unsuspend();
//		sbs.erase(u);
//	}
//}


/********************************************/
/* PurchaseBehaviour : DispatchingBehaviour */
/********************************************/

PurchaseBehaviour::PurchaseBehaviour(Unit *_cust, Unit *_staff, Furnishing *_f, Level *_level) :
	customer(_cust), staff(_staff), furnishing(_f), level(_level)
{
	printf("PurchaseBehaviour %p created on customer %p, shopkeeper %p and furnishing %p\n", this, customer, staff, furnishing);
	customer->capture();
	staff->capture();
	furnishing->capture();
}
void PurchaseBehaviour::_update() {
	if (stage == 0) {
		// Send units to interaction points
		W::position uDest, sDest;
		furnishing->getInteractionPoint("customer", uDest);	// NB: may throw
		furnishing->getInteractionPoint("staff", sDest);	//
		if (customer->voyage(uDest) && staff->voyage(sDest)) stage++;
		else wait();
	}
	else if (stage == 1) {
		// Run animations
		if (customer->arrived && staff->arrived)
			customer->runAnimation(), staff->runAnimation(), stage++;
	}
	else if (stage == 2) {
		// Finish
		if (customer->animation_finished && staff->animation_finished) {
			MrKlangy::playSound("cha-ching.wav");
			level->payPlayer(3);	// TODO: charge the price of the item being purchased
			customer->release(), staff->release(), furnishing->release();
			destroy();
		}
	}
}


/* ServiceBehaviour */

//bool ServiceBehaviour::lua_initialized = false;
//std::map<std::string, serviceBehaviourInfo> ServiceBehaviour::types;
//
//ServiceBehaviour::ServiceBehaviour(const char *_type, W::EventHandler *_eh) :
//	Behaviour(_eh), type(_type)
//{
//	bType = &types[type];
//}
//ServiceBehaviour::~ServiceBehaviour()
//{
//	eh->unsubscribeFromEventType(W::EventType::INTERRUPT_UNITPICKUP, this);
//}
//void ServiceBehaviour::init(Level *_l, Unit *_u, Unit *_s, Furnishing *_f) {
//	level = _l, unit = _u, staff = _s, furnishing = _f;
//	// Check furnishing is capable of this interaction
//	if (!furnishing->capableOfBehaviour(type.c_str())) {
//		char s[150]; sprintf(s, "'%s' behaviour initialized with incompatible '%s' furnishing", type.c_str(), furnishing->type.c_str());
//		throw W::Exception(s);
//	}
//	unit->capture(), staff->capture(), furnishing->capture();
//	initialized = true;
//	
//	eh->subscribeToEventType(
//		W::EventType::INTERRUPT_UNITPICKUP,
//		W::Callback(&ServiceBehaviour::receiveEvent, this)
//	);
//}
//void ServiceBehaviour::_update() {
//
//	// Interrupt handling
//	else if (stage == 10) {	// Send unit to queue point
//		int x, y;
//		char s[120]; sprintf(s, "interrupt: getting queue point from building %p", contextBuilding);
//		std::cout << s << std::endl;
//		contextBuilding->getQueuePoint(&x, &y);
//		if (unit->voyage(x, y)) stage++;
//		else wait();
//	}
//	else if (stage == 11) {	// Done!
//		if (unit->arrived) {
//			unit->nextBehaviour = type;
//			unit->release();
//			contextBuilding->addToQueue(unit);
//			destroyed = true;
//		}
//	}
//}
//void ServiceBehaviour::receiveEvent(W::Event *ev) {
//	if (ev->type == W::EventType::INTERRUPT_UNITPICKUP) {
//		if (ev->_payload != staff)
//			return;
//		// Interrupt the behaviour
//		contextBuilding = staff->getContextBuilding();
//		staff->release(), furnishing->release();
//		stage = 10;
//	}
//}
//bool ServiceBehaviour::initialize() {
//	lua_State *L = mrLua->LuaInstance;
//	
//	// Construct types map
//	if (!mrLua->pushtable("serviceBehaviours")) {
//		W::log << "In behaviours.lua, could not push the serviceBehaviours table onto the stack" << std::endl;
//		return false;
//	}
//	lua_pushnil(L);							// S: -1 nil; -2 serviceBehaviours
//	while (lua_next(L, -2)) {				// S: -1 behaviour; -2 key; -3 serviceBehaviours
//		if (!lua_istable(L, -1)) {		// If not a subtable, skip
//			lua_pop(L, 1);
//			continue;
//		}
//		const char *bType = lua_tostring(L, -2);
//		struct serviceBehaviourInfo *bInfo = &ServiceBehaviour::types[bType];
//		
//		try {
//			bInfo->charge = mrLua->getfield<int>("charge");
//		}
//		catch (W::Exception &exc) {
//			W::log << "Invalid ServiceBehaviour definition in behaviours.lua for '" << bType << "' type" << std::endl;
//			return false;
//		}
//		
//		lua_pop(L, 1);						// S: -1 key; -2 serviceBehaviours
//	}										// S: -1 serviceBehaviours
//	lua_pop(L, 1);							// S: ~
//	
//	ServiceBehaviour::lua_initialized = true;
//	return true;
//}
