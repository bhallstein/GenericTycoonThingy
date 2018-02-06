/*
 * Generic Tycoon Thingy
 *
 * ==================
 *  Furnishing.cpp
 * ==================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#include "Furnishing.hpp"
#include "Building.hpp"
#include "Unit.hpp"
#include "MrPaths.hpp"
#include "W.h"
#include "Placeable.hpp"
#include "LevelView.hpp"
#include "LevelMap.hpp"
#include "Serializer.hpp"
#include <algorithm>

/* furnishingInfo constructor impl */

furnishingInfo::furnishingInfo(LuaObj &o) {
	LuaObj *l;
	
	// Cost (optional: def. 0)
	l = &o["cost"];
	cost = (o["cost"].isNumber() ? o["cost"].number_value : 0);
	
	// Plan
	l = &o["plan"];
	if (!l->isTable()) {
		throw W::Exception("Error creating furnishingInfo: 'plan' not found or not table");
	}
	auto &descs_plan = l->descendants;
	for (auto it = descs_plan.begin(); it != descs_plan.end(); ++it) {
		LuaObj &planComponent = it->second;
		LuaObj &posObj = planComponent["0"], &szObj = planComponent["1"];
		plan.push_back(W::rect(
			W::position((int)posObj["0"].number_value, (int)posObj["1"].number_value),
			W::size(szObj["0"].number_value, szObj["1"].number_value)
		));
	}
	
	// Seekables (compatible seek targets)
	l = &o["seekables"];
	if (l->isTable()) {
		auto &descs_seekables = l->descendants;
		for (auto it = descs_seekables.begin(); it != descs_seekables.end(); ++it) {
			LuaObj &seekable = it->second;
			if (!seekable.isString()) {
				throw W::Exception("Error creating furnishingInfo: invalid (non-string) seekable");
			}
			SeekTarget::Type seekable_type;
			_deserialize(seekable, seekable_type);
			seekables.push_back(seekable_type);
		}
	}
}


/* Furnishing: static properties */

std::map<std::string, furnishingInfo*> Furnishing::furnishingTypeInfo;
Serializable::serialization_descriptor Furnishing::sd;
bool Furnishing::initialized = false;


/*** Furnishing ***/

Furnishing::Furnishing(LevelMap *_lm, LevelView *_lv, W::NavMap *_nm, bool _placeableMode) :
	PlaceableManager(_lm, _lv, _nm, _placeableMode),
	purchased(false)
{
	rct.sz = W::size(2,2);

	drawnFurnishing = new DrawnFurnishing(levelView);
	drawnFurnishing->setPosn(rct.pos);
	
	W::Messenger::subscribeInView(levelView, W::EventType::LMouseUp, W::Callback(&Furnishing::mouseEvent, this), &rct);
}
Furnishing::~Furnishing()
{
	std::cout << "furnishing destruct" << std::endl;
	using namespace W::EventType;
	W::Messenger::unsubscribeInView(levelView, LMouseUp, this);
//	W::Messenger::unsubscribeFromEventType(LV_LEFTMOUSEDOWN,  this);
//	W::Messenger::unsubscribeFromEventType(LV_LEFTMOUSEUP,    this);
//	W::Messenger::unsubscribeFromEventType(LV_RIGHTMOUSEDOWN, this);
//	W::Messenger::unsubscribeFromEventType(LV_RIGHTMOUSEUP,   this);
//	W::Messenger::unsubscribeFromEventType(LV_MOUSEMOVE,      this);
	navmap->makePassable(rct);
	delete drawnFurnishing;
}

void Furnishing::_setUp() {
	if (type == NO_TYPE) {
		throw W::Exception("setUp() called on Furnishing with type NO_TYPE. Call setType() or deserialize first.");
	}
	
	// Set typeInfo pointer
	auto it = furnishingTypeInfo.find(type);
	if (it == furnishingTypeInfo.end()) {
		throw W::Exception(std::string("Info for furnishing type \"") + type + "\" not found");
	}
	typeInfo = Furnishing::furnishingTypeInfo[type];
	
	// Perform set-up for furnishings constructed programmatically
	if (!deserialized) {
		// ...
	}
	
	// Set up state of DrawnFurnishing
	if (type == "piecounter") {
		drawnFurnishing->setColour(W::Colour::Yellow);
	}
	else {
		drawnFurnishing->setColour(W::Colour::Pink);
	}
}

W::EventPropagation::T Furnishing::mouseEvent(W::Event *ev) {
	using namespace W::EventType;
	
	if (ev->type == MouseMove) { /* hover = true; */ }
	else if (ev->type == LMouseUp) {
		std::cout << "furnishing " << uid.id << " detected left mouse up" << std::endl;
	}
	
	return W::EventPropagation::ShouldStop;
}

void Furnishing::update() {

}

bool Furnishing::initialize() {
	if (Furnishing::initialized) return true;
	
	// 1. Get furnishingTypeInfo LuaObj
	
	std::string path = MrPaths::resourcesPath + "Data/Object info/furnishings.lua";
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
	for (auto it = o.descendants.begin(); it != o.descendants.end(); ++it) {
		furnishingTypeInfo[it->first] = new furnishingInfo(it->second);
	}
	
	// 2. Set up Serialization Descriptor
	sd["purchased"] = makeSerializer(&Furnishing::purchased);
	
	Furnishing::initialized = true;
	return true;
}

std::vector<std::string> Furnishing::get_furnishing_types() {
  std::vector<std::string> out;
  for (auto it : furnishingTypeInfo) {
    out.push_back(it.first);
  }
  return out;
}
int Furnishing::costForType(const char *_type) {
	return furnishingTypeInfo[_type]->cost;
}
bool Furnishing::supports_seekTarget(SeekTarget::Type targ) const {
	auto types = typeInfo->seekables;
	return std::find(types.begin(), types.end(), targ) != types.end();
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
		levelMap->addPlayerMoneys(-typeInfo->cost);
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
	r = new W::DRect(
		lv, W::position(), lv->convertGridToPixelCoords(W::size(2,2)), W::Colour::Blue
	);
}
Furnishing::DrawnFurnishing::~DrawnFurnishing()
{
	delete r;
}
void Furnishing::DrawnFurnishing::setPosn(const W::position &p) {
	r->setPos(lv->convertGridToPixelCoords(p));
}
void Furnishing::DrawnFurnishing::setOpac(float x) {
	W::Colour c = r->col;
	c.a = x;
	r->setCol(c);
}
void Furnishing::DrawnFurnishing::setColour(W::Colour _col) {
	float alpha = r->col.a;
	r->col = _col;
	r->col.a = alpha;
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
