/*
 * Generic Tycoon Thingy
 *
 * ================
 *  LevelMap.cpp
 * ================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#include "LevelMap.hpp"
#include "LevelView.hpp"
#include "TLO.hpp"
#include "LuaObj.h"
#include "Unit.hpp"
#include "Furnishing.hpp"
#include "Building.hpp"
#include <algorithm>

LevelMap::LevelMap(LevelState *_ls, LevelView *_lv) :
	levelState(_ls),
	levelView(_lv),
	loaded(false)
{
	W::Messenger::subscribe(W::EventType::KeyUp, W::Callback(&LevelMap::keyEvent, this));
}
LevelMap::~LevelMap()
{
	// bai
}

void LevelMap::update(int frame_microseconds, float time_in_level) {
	// Spawn units
	// The % chance of spawning a unit is kT, where T is the current fraction of the level time limit
  // Note: this chance is per-frame, so depends on the framerate, which is bad.
  float k = 0.2;
  float p_spawn = k * time_in_level / timeLimit;
  if (W::Rand::intUpTo(1000) < p_spawn * 1000) {
    createUnit(false, "customer", map__randomCoord());
    printf("%lu units, %lu controllers\n", units.size(), controllers.size());
  }

//  if (first_frame) {
//    for (int i=0; i < 100; ++i) {
//      createUnit(false, "customer", map__randomCoord());
//    }
//
//    first_frame = false;
//  }

	// Update object vectors
	tlovec__update(units);
	tlovec__update(furnishings);
	tlovec__update(controllers);
	tlovec__update(buildings);
	
	// Update controllers
	for (auto it = controllers_to_deactivate.begin(); it < controllers_to_deactivate.end(); ++it) {
		for (auto it_active = controllers.begin(); it_active < controllers.end(); ) {
			if (*it == *it_active) { it_active = controllers.erase(it_active); }
			else ++it_active;
		}
	}
	controllers_to_deactivate.clear();
	for (auto it = controllers_to_reactivate.begin(); it < controllers_to_reactivate.end(); ++it) {
		controllers.push_back(*it);
	}
	controllers_to_reactivate.clear();
	
	// Clear destroyed items
	tlovec__clearDestroyeds(units);
	tlovec__clearDestroyeds(furnishings);
	tlovec__clearDestroyeds(controllers);
	tlovec__clearDestroyeds(buildings);
}

W::EventPropagation::T LevelMap::keyEvent(W::Event *ev) {
	if (ev->key == W::KeyCode::_U) createUnit(true, "customer", W::position());
	else if (ev->key == W::KeyCode::_K) createUnit(true, "shopkeeper", W::position());
	else if (ev->key == W::KeyCode::_A) createFurnishing(true, "barberschair", W::position());
	else if (ev->key == W::KeyCode::_B) createFurnishing(true, "piecounter", W::position());
	return W::EventPropagation::ShouldContinue;
}

bool LevelMap::load(lua_State *L) {
	if (loaded) {
		return false;
	}
	
	// Get LuaObjs from file
	lua_getglobal(L, "levelData");   LuaObj levelData(L);
	lua_getglobal(L, "playerState"); LuaObj playerState(L);
	lua_getglobal(L, "mapData");     LuaObj mapData(L);
	lua_close(L);
	
	if (!levelData.isTable()) {
		W::log << "Error: levelData table not found" << std::endl;
		return false;
	}
	if (!playerState.isTable()) {
		W::log << "Error: playerState table not found" << std::endl;
		return false;
	}
	if (!mapData.isTable()) {
		W::log << "Error: mapData table not found" << std::endl;
		return false;
	}
	
	
	
	// Get width & height
	LuaObj &wObj = mapData["width"];
	LuaObj &hObj = mapData["height"];
	if (!wObj.isNumber()) {
		W::log << "mapData.width not found" << std::endl;
		return false;
	}
	if (!hObj.isNumber()) {
		W::log << "mapData.height not found" << std::endl;
		return false;
	}
	mapSize = W::size(wObj.number_value, hObj.number_value);
	levelView->setLevelSize(mapSize);
	navmap = new W::NavMap(mapSize);
	
	/* Load TLOs */

	UIDManager::newBatch();
		// This call is important: it resets UIDManager’s UID translation table –
		// necessary since when deserialized, all UIDs are auto-translated to "fresh"
		// UIDs, via said table.
	LuaObj::_descendantmap *d;
	
	// Get furnishings
	LuaObj &furnishingsObj = mapData["furnishings"];
	if (!furnishingsObj.isTable()) {
		W::log << "mapData.furnishings table not found" << std::endl;
		return false;
	}
	d = &furnishingsObj.descendants;
	for (LuaObj::_descendantmap::iterator it = d->begin(); it != d->end(); ++it) {
		createFurnishing(it->second);
	}
	
	// Get units
	LuaObj &unitsObj = mapData["units"];
	if (!unitsObj.isTable()) {
		W::log << "mapData.units table not found" << std::endl;
		return false;
	}
	d = &unitsObj.descendants;
	for (LuaObj::_descendantmap::iterator it = d->begin(); it != d->end(); ++it) {
		createUnit(it->second);
	}
	
	// Get controllers
	LuaObj &controllersObj = mapData["controllers"];
	d = &controllersObj.descendants;
	for (LuaObj::_descendantmap::iterator it = d->begin(); it != d->end(); ++it) {
		createController(it->second, true);
	}
	
	// Get inactive controllers
	LuaObj &inactiveControllersObj = mapData["inactiveControllers"];
	d = &inactiveControllersObj.descendants;
	for (LuaObj::_descendantmap::iterator it = d->begin(); it != d->end(); ++it) {
		createController(it->second, false);
	}
	
	// Get buildings
	LuaObj &buildingsObj = mapData["buildings"];
	if (buildingsObj.isTable()) {
		d = &buildingsObj.descendants;
		for (LuaObj::_descendantmap::iterator it = d->begin(); it != d->end(); ++it) {
			createBuilding(it->second);
		}
	}
	
	
	/* Load player & level data */
	// Level Data
	LuaObj &monetaryTargetObj = levelData["monetaryTarget"];
	if (!monetaryTargetObj.isNumber()) {
		W::log << "levelData.monetaryTarget not found or not number" << std::endl;
		return false;
	}
	monetaryTarget = monetaryTargetObj.number_value;
	LuaObj &timeLimitObj = levelData["timeLimit"];
	if (!timeLimitObj.isNumber()) {
		W::log << "levelData.timeLimit not found or not number" << std::endl;
		return false;
	}
	timeLimit = timeLimitObj.number_value;
	
	// Player State
	LuaObj &balanceObj = playerState["balance"];
	if (!balanceObj.isNumber()) {
		W::log << "playerState.balance not found or not number" << std::endl;
		return false;
	}
	playerMoneys = balanceObj.number_value;
	
	return loaded = true;
}

std::string LevelMap::save() {
	using std::string;
	std::stringstream ss;
	
	// Level Data
	ss << "levelData = {\n";
	ss << "monetaryTarget = " << monetaryTarget << ",\n";
	ss << "timeLimit = " << timeLimit << "\n";
	ss << "}\n\n";
	
	// Player State
	ss << "playerState = {\n";
	ss << "balance = " << playerMoneys << "\n";
	ss << "}\n\n";
	
	// Map contents
	ss << "mapData = {\n";
	
	ss << "width = " << mapSize.width << ",\n";
	ss << "height = " << mapSize.height << ",\n\n";
	
	// Units
	ss << "units = {\n";
	if (units.size() == 0) ss << "\n";
	for (tlovec::iterator it = units.begin(), itend = units.end(); it < itend; ++it)
		ss << "{\n" << (*it)->serialize() << (it == itend-1 ? "}\n" : "},\n");
	ss << "},\n";
	
	// Furnishings
	ss << "furnishings = {\n";
	if (furnishings.size() == 0) ss << "\n";
	for (tlovec::iterator it = furnishings.begin(), itend = furnishings.end(); it < itend; ++it)
		ss << "{\n" << (*it)->serialize() << (it == itend-1 ? "}\n" : "},\n");
	ss << "},\n";
	
	// Controllers
	ss << "controllers = {\n";
	if (controllers.size() == 0) ss << "\n";
	for (tlovec::iterator it = controllers.begin(), itend = controllers.end(); it < itend; ++it)
		ss << "{\n" << (*it)->serialize() << (it == itend-1 ? "}\n" : "},\n");
	ss << "},\n";
	
	// Inactive Controllers
	ss << "inactiveControllers = {\n";
	if (inactiveControllers.size() == 0) ss << "\n";
	for (tlovec::iterator it = inactiveControllers.begin(), itend = inactiveControllers.end(); it < itend; ++it)
		ss << "{\n" << (*it)->serialize() << (it == itend-1 ? "}\n" : "},\n");
	ss << "}\n";
	
	ss << "}\n";
	
	return ss.str();
}

Furnishing* LevelMap::createFurnishing(bool placeableMode, const std::string &type, const W::position &pos) {
	Furnishing *f = new Furnishing(this, levelView, navmap, placeableMode);
	f->setType(type);
	f->setUp();
	bool initsuccess = f->init(placeableMode ? W::position(-100,-100) : pos);
	if (initsuccess) {
		furnishings.push_back(f);
		W::log << "Furnishing " << f->uid.id << " created and initialized "
			<< (placeableMode ? " in placeable mode" : std::string("at ") + f->rct.pos.xyStr()) << std::endl;
	}
	else {
		delete f;
		f = NULL;
	}
	return f;
}
Furnishing* LevelMap::createFurnishing(LuaObj &o) {
	Furnishing *f = new Furnishing(this, levelView, navmap, false);
	f->deserialize(o);
	f->setUp();
	if (f->init()) {
		furnishings.push_back(f);
		W::log << "Furnishing " << f->uid.id << " deserialized and initialized at " << f->rct.pos.xyStr() << std::endl;
	}
	else {
		W::log << "Furnishing " << f->uid.id << " deserialized then failed to initialize at " << f->rct.pos.xyStr() << std::endl;
		delete f;
		f = NULL;
	}
	return f;
}
Building* LevelMap::createBuilding(const std::string &type, const W::position &pos) {
	Building *b = new Building(this, levelView, navmap);
	b->setType(type);
	b->setUp();
	b->setPos(pos);
	buildings.push_back(b);
	return b;
}
Building* LevelMap::createBuilding(LuaObj &o) {
	Building *b = new Building(this, levelView, navmap);
	b->deserialize(o);
	b->setUp();
	buildings.push_back(b);
	return b;
}

Unit* LevelMap::createUnit(bool placeableMode, const std::string &type, const W::position &pos) {
	Unit *u = new Unit(this, levelView, navmap, placeableMode);
	u->setType(type);
	u->setUp();
	bool initsuccess = u->init(placeableMode ? W::position(-100,-100) : pos);
	if (initsuccess) {
		units.push_back(u);
		W::log << "Unit (" << u->type << ") " << u->uid.id << " created and initialized "
			<< (placeableMode ? "in placeable mode" : std::string("at ") + u->rct.pos.xyStr()) << std::endl;
		Controller *c = createControllerForUnit(u);
		c->dispatchUnit(u);
	}
	else {
		W::log << "Unit (" << u->type << ") " << u->uid.id << " created then failed to initialize" << std::endl;
		delete u;
		u = NULL;
	}
	return u;
}
Unit* LevelMap::createUnit(LuaObj &o) {
	Unit *u = new Unit(this, levelView, navmap, false);
	u->deserialize(o);
	u->setUp();
	if (u->init()) {
		units.push_back(u);
		W::log << "Unit (" << u->type << ") " << u->uid.id << " deserialized and initialized at " << u->rct.pos.xyStr() << std::endl;
	}
	else {
		W::log << "Unit (" << u->type << ") " << u->uid.id << " deserialized then failed to initialize at " << u->rct.pos.xyStr() << std::endl;
		delete u;
		u = NULL;
	}
	return u;
}
SeekTarget::Type LevelMap::unitSeekTarget() {
	return (SeekTarget::Type) W::Rand::intUpTo(SeekTarget::__N);
}
Controller* LevelMap::createController(const std::string &type, bool active) {
	Controller *c = NULL;
	if (type == "CustomerController") {
		c = new CustomerController(this, levelView, navmap, unitSeekTarget());
	}
	else if (type == "ShopkeeperController") {
		c = new ShopkeeperController(this, levelView, navmap);
	}
	if (c) {
		c->setUp();
		(active ? controllers : inactiveControllers).push_back(c);
		W::log << "Controller (" << type << ") " << c->uid.id << " created" << std::endl;
	}
	return c;
}
Controller* LevelMap::createController(LuaObj &o, bool active) {
	Controller *c = NULL;
	const std::string &type = o["type"].str_value;
	if (type == "CustomerController") {
		c = new CustomerController(this, levelView, navmap, unitSeekTarget());
	}
	else if (type == "ShopkeeperController") {
		c = new ShopkeeperController(this, levelView, navmap);
	}
	if (c != NULL) {
		c->deserialize(o);
		c->setUp();
		(active ? controllers : inactiveControllers).push_back(c);
		W::log << "Controller (" << type << ") " << c->uid.id << " deserialized" << std::endl;
	}
	return c;
}
Controller* LevelMap::createControllerForUnit(Unit *u) {
	using std::string;
	
	const string &type = u->type;
	Controller *c = NULL;
	if (type == "customer") c = createController("CustomerController");
	else if (type == "shopkeeper") c = createController("ShopkeeperController");
	
	if (c == NULL) throw W::Exception(
		string("Error: couldn't create controller for unit - unrecognised type: ") +
		string("'") + type + string("'")
	);
	
	return c;
}

Building* LevelMap::building__getRandom() {
	if (buildings.size() == 0) {
		return NULL;
	}
	return (Building*) buildings[W::Rand::intUpTo((int) buildings.size())];
}
Building* LevelMap::building__findAt(W::position &p) {
	for (auto it = buildings.begin(); it != buildings.end(); ++it) {
		TLO *b = *it;
		if (b->rct.overlapsWith(p)) {
			return (Building*) b;
		}
	}
	return NULL;
}
Building* LevelMap::building__withFurnishingSupportingSeekTarget(SeekTarget::Type seek_target) {
	// Find a furnishing supporting the SeekTarget
	std::vector<TLO*> supporting_furnishings;
	std::copy_if(furnishings.begin(), furnishings.end(), std::back_inserter(supporting_furnishings), [=](const TLO *x) {
		Furnishing *f = (Furnishing*) x;
		return f->supports_seekTarget(seek_target);
	});
	
	int n_found = (int) supporting_furnishings.size();
	if (n_found == 0) {
		return NULL;
	}
	
	TLO *f = supporting_furnishings[W::Rand::intUpTo(n_found)];
	return building__findAt(f->rct.pos);
}

W::position LevelMap::map__randomCoord() {
	return W::position(
		W::Rand::intUpTo(width()),
		W::Rand::intUpTo(height())
	);
}

void LevelMap::deactivateController(Controller *c) {
	printf("Deactivate controller %p\n", c);
	controllers_to_deactivate.push_back(c);
}
void LevelMap::reactivateController(Controller *c) {
	printf("Reactivate controller %p\n", c); 
	controllers_to_reactivate.push_back(c);
}

bool LevelMap::addPlayerMoneys(int x) {
	int result = playerMoneys + x;
	if (result < 0) {
		return false;
	}
	playerMoneys = result;
	return true;
}

void LevelMap::tlovec__update(tlovec &v) {
	for (auto it = v.begin(); it != v.end(); ++it) {
		(*it)->update();
	}
}
void LevelMap::tlovec__clearDestroyeds(tlovec &v) {
	for (auto it = v.begin(); it != v.end(); ) {
		TLO *tlo = *it;
		if (tlo->destroyed) {
			delete tlo;
			it = v.erase(it);
		}
		else {
			++it;
		}
	}
}
