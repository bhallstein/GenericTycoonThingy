#include "Furnishing.hpp"
#include "Building.hpp"
#include "Unit.hpp"
#include "MrPaths.hpp"
#include "W.h"
#include "Placeable.hpp"
#include "LevelView.hpp"
#include "LevelState.hpp"

/* furnishingInfo constructor impl */

furnishingInfo::furnishingInfo(LuaObj &o) {
	LuaObj *l;
	
	// Cost (optional: def. 0)
	l = &o["cost"];
	cost = (o["cost"].isNumber() ? o["cost"].number_value : 0);
	
	// Plan
	l = &o["plan"];
	if (!l->isTable()) throw W::Exception("Error creating furnishingInfo: 'plan' not found or not table");
	LuaObj::_descendantmap &desc = l->descendants;
	for (LuaObj::_descendantmap::iterator it = desc.begin(); it != desc.end(); ++it) {
		LuaObj &planComponent = it->second;
		LuaObj &posObj = planComponent["0"], &szObj = planComponent["1"];
		plan.push_back(W::rect(
			W::position((int)posObj["0"].number_value, (int)posObj["1"].number_value),
			W::size(szObj["0"].number_value, szObj["1"].number_value)
		));
	}
}


/* Furnishing: static properties */

std::map<std::string, furnishingInfo*> Furnishing::furnishingTypeInfo;
Serializable::serialization_descriptor Furnishing::sd;
bool Furnishing::initialized = false;


/*** Furnishing ***/

Furnishing::Furnishing(LevelState *_ls, LevelMap *_lm, LevelView *_lv, W::NavMap *_nm, bool _placeableMode) :
	PlaceableManager(_ls, _lm, _lv, _nm, _placeableMode),
	purchased(false)
{
	rct.sz = W::size(2,2);
	// Create DrawnFurnishing
	drawnFurnishing = new DrawnFurnishing(levelView);
	drawnFurnishing->setPosn(rct.pos);
	
	W::Messenger::subscribeToPositionalEventType(W::EventType::LV_LEFTMOUSEUP, W::Callback(&Furnishing::mouseEvent, this), &rct);
	std::cout << "lv left mouse up: " << W::EventType::LV_LEFTMOUSEUP << "\n";
}
Furnishing::~Furnishing()
{
	std::cout << "furnishing destruct" << std::endl;
	using namespace W::EventType;
	W::Messenger::unsubscribeFromEventType(LV_LEFTMOUSEDOWN,  this);
	W::Messenger::unsubscribeFromEventType(LV_LEFTMOUSEUP,    this);
	W::Messenger::unsubscribeFromEventType(LV_RIGHTMOUSEDOWN, this);
	W::Messenger::unsubscribeFromEventType(LV_RIGHTMOUSEUP,   this);
	W::Messenger::unsubscribeFromEventType(LV_MOUSEMOVE,      this);
	navmap->makePassable(rct);
	UIDManager::unregisterTLO(this);
	delete drawnFurnishing;
}

void Furnishing::setUp() {
	using std::string;
	
	if (type == NO_TYPE)
		throw W::Exception("setUp() called on Furnishing with type NO_TYPE. Call setType() or deserialize first.");
	
	// Set typeInfo pointer
	std::map<string,furnishingInfo*>::iterator it = furnishingTypeInfo.find(type);
	if (it == furnishingTypeInfo.end()) {
		string msg = string("Info for furnishing type \"") + type + "\" not found";
		throw W::Exception(msg);
	}
	typeInfo = Furnishing::furnishingTypeInfo[type];
	
	// Perform set-up for furnishings constructed programmatically
	if (!deserialized) {
		uid = UIDManager::getNewUID();
	}
	UIDManager::registerTLO(this);
	
	// Set up state of DrawnFurnishing
	// ...
}

W::EventPropagation::T Furnishing::mouseEvent(W::Event *ev) {
	using namespace W::EventType;
	
	if (ev->type == LV_MOUSEMOVE) { /* hover = true; */ }
	else if (ev->type == LV_LEFTMOUSEUP)
		std::cout << "furnishing " << uid.id << " detected left mouse up" << std::endl;
	
	return W::EventPropagation::SHOULD_STOP;
}

void Furnishing::update() {
//	if (animating) {
//		if (++animFrames == animationDuration)
//			animating = false, animationFinished = true;
//	}
}

bool Furnishing::initialize() {
	if (Furnishing::initialized) return true;
	
	// 1. Get furnishingTypeInfo LuaObj
	
	std::string path = MrPaths::resourcesPath + "Lua/furnishings.lua";
	lua_State *L;
	if (!luaLoad(path, &L)) {
		W::log << "Could not read furnishings.lua" << std::endl;
		return false;
	}
	lua_getglobal(L, "furnishingTypes"); LuaObj o(L);
	lua_close(L);
	if (!o.isTable()) {
		W::log << "Could not get furnishingTypes table from furnishings.lua" << std::endl;
		return false;
	}
	for (LuaObj::_descendantmap::iterator it = o.descendants.begin(); it != o.descendants.end(); ++it)
		furnishingTypeInfo[it->first] = new furnishingInfo(it->second);
	
	// 2. Set up Serialization Descriptor
	sd["purchased"] = makeSerializer(&Furnishing::purchased);
	
	Furnishing::initialized = true;
	return true;
}
int Furnishing::costForType(const char *_type) {
	return furnishingTypeInfo[_type]->cost;
}

/* PlaceableManager overrides */

void Furnishing::placementLoopStarted() {
	drawnFurnishing->setOpac(0.2);		// Put DF in placement-loop mode
	navmap->makePassable(rct);			// Make previous location passable
}
void Furnishing::placementLoopUpdate() {
	drawnFurnishing->setPosn(placeable.pos);
}
void Furnishing::placementLoopCancelled() {
	// If were purchasing a new furnishing: destroy it
	if (!purchased) destroy();
	
	// Put the furnishing back where we found it
	drawnFurnishing->setPosn(rct.pos);
	navmap->makeImpassable(rct);
	
	// Put DrawnFurnishing back in normal mode
	drawnFurnishing->setOpac(1);
}
void Furnishing::placementLoopSucceeded() {
	// If purchasing: charge player
	if (!purchased) {
		levelState->addPlayerMoneys(-typeInfo->cost);
		purchased = true;
	}
	navmap->makeImpassable(rct);
	drawnFurnishing->setPosn(rct.pos);	// Update DrawnFurnishing to new position
	drawnFurnishing->setOpac(1);		// Put DF back in normal mode
}
bool Furnishing::canPlace(const W::position &_pos) {
	// Can place if:
	//  - passable underneath
	//  - *in future, no units underneath
	//  - *perhaps in future, is entirely inside its context room/building
	W::position prev_pos = rct.pos;
	rct.setPos(_pos);
	bool can_place;
	can_place = navmap->isPassableUnder(rct);
	rct.setPos(prev_pos);
	
	return can_place;
}


/* DrawnFurnishing impl */

Furnishing::DrawnFurnishing::DrawnFurnishing(LevelView *_lv) : lv(_lv)
{
	r = new W::DrawnRect(
		lv, W::position(), lv->convertGridToPixelCoords(W::size(2,2)), W::Colour::Blue
	);
	lv->addDO(r);
}
Furnishing::DrawnFurnishing::~DrawnFurnishing()
{
	lv->removeDO(r);
}
void Furnishing::DrawnFurnishing::setPosn(const W::position &p) {
	r->setPos(lv->convertGridToPixelCoords(p));
}
void Furnishing::DrawnFurnishing::setOpac(float x) {
	W::Colour c = r->col;
	c.a = x;
	r->setCol(c);
}


//Door::Door(W::NavMap *_nm, const char *_type, Level *_level, bool _placeableMode) :
//	Furnishing(_nm, _type, _level, NULL, _placeableMode)
//{
//	// ello
//}
//Door::~Door()
//{
//	// bai doory
//}
//
//bool Door::canPlace(int _x, int _y) {
//	return true;
//}
//void Door::finalizePlacement() {
//	// Recreate door connections
//	// Get door coordinates
//	std::vector<W::position> dcoords;
//	W::rect drect = plan[0];
//	for (int i = pos.x + drect.pos.x; i < pos.x + drect.pos.x + drect.sz.width; i++)
//		for (int j = pos.y + drect.pos.y; j < pos.y + drect.pos.y + drect.sz.height; j++)
//			dcoords.push_back(W::position(i, j));
//	if (dcoords.size() != 2)
//		W::log << "Door found with " << dcoords.size() << " coordinates in plan!" << std::endl;
//	else
//		navmap->createConnection(dcoords[0], dcoords[1]);
//	
//	return;
//}
