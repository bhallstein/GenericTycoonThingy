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
#include "Views__UI.hpp"
#include "TLO.hpp"
#include "LuaObj.h"
#include "Unit.hpp"
#include "Furnishing.hpp"
#include "Building.hpp"
#include "State__WinLose.hpp"
#include <algorithm>

LevelMap::LevelMap(State__Game *_ls, LevelView *_lv, View__BottomBar *_view__btmBar) :
	state__game(_ls),
	view__level(_lv),
  view__btmBar(_view__btmBar),
	loaded(false)
{
	W::Messenger::subscribe(W::EventType::KeyUp, W::Callback(&LevelMap::keyEvent, this));

  W::Messenger::subscribeToUIEvent("hire_staff", W::EventType::ButtonClick, W::Callback(&LevelMap::buttonEvent, this));
  W::Messenger::subscribeToUIEvent("buy_furnishing:barberschair", W::EventType::ButtonClick, W::Callback(&LevelMap::buttonEvent, this));
  W::Messenger::subscribeToUIEvent("buy_furnishing:piecounter", W::EventType::ButtonClick, W::Callback(&LevelMap::buttonEvent, this));

  W::Messenger::subscribe(W::EventType::Haircut, W::Callback(&LevelMap::economicEvent, this));
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
  float p_spawn = k * time_in_level / level_time_limit_s;
  if (W::Rand::intUpTo(1000) < p_spawn * 1000) {
    Building *b = map__randomBuilding("home");
    if (b) {
      createUnit(false, "customer", b->centrePoint());
      printf("%lu units, %lu controllers\n", units.size(), controllers.size());
    }
  }

  // Set time remaining
  int time_remaining_s = level_time_limit_s - int(time_in_level);
  view__level->setRemainingTime(time_remaining_s);

  // Set economy
  view__btmBar->setEcon(cash);

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

  // Check for win condition
  if (cash >= 1000) {
    W::pushState(new State__WinLose());
  }
}

W::EventPropagation::T LevelMap::keyEvent(W::Event *ev) {
	if (ev->key == W::KeyCode::_U) createUnit(true, "customer", W::position());
	else if (ev->key == W::KeyCode::_K) createUnit(true, "shopkeeper", W::position());
	else if (ev->key == W::KeyCode::_A) createFurnishing(true, "barberschair", W::position());
	else if (ev->key == W::KeyCode::_B) createFurnishing(true, "piecounter", W::position());
  else if (ev->key == W::KeyCode::_W) W::pushState(new State__WinLose());
	return W::EventPropagation::ShouldContinue;
}

W::EventPropagation::T LevelMap::buttonEvent(W::Event *ev) {
  std::string *name = (std::string*) ev->_payload;

  if (*name == "hire_staff") {
//    buyUnit("shopkeeper");
    createUnit(true, "shopkeeper", {-1,-1});
  }
  else if (*name == "buy_furnishing:barberschair") {
    createFurnishing(true, "barberschair", {-1,-1});
  }
  else if (*name == "buy_furnishing:piecounter") {
    createFurnishing(true, "piecounter", {-1,-1});
  }

  return W::EventPropagation::ShouldContinue;
}

W::EventPropagation::T LevelMap::economicEvent(W::Event *ev) {
  if (ev->type == W::EventType::Haircut) {
    addPlayerMoneys(Furnishing::costForType("barberschair"));
  }
  else if (ev->type == W::EventType::PieSale) {
    addPlayerMoneys(Furnishing::costForType("piecounter"));
  }

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
	view__level->setLevelSize(mapSize);
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
	LuaObj &obj__financial_target = levelData["financialTarget"];
	if (!obj__financial_target.isNumber()) {
		W::log << "levelData.financialTarget not found or not number" << std::endl;
		return false;
	}
	level_financial_target = obj__financial_target.number_value;

  LuaObj &obj__time_limit = levelData["timeLimit"];
	if (!obj__time_limit.isNumber()) {
		W::log << "levelData.timeLimit not found or not number" << std::endl;
		return false;
	}
	level_time_limit_s = obj__time_limit.number_value;
	
	// Player State
	LuaObj &obj__cash = playerState["cash"];
	if (!obj__cash.isNumber()) {
		W::log << "playerState.cash not found or not number" << std::endl;
		return false;
	}
	cash = obj__cash.number_value;
  view__btmBar->setEcon(cash);
	
	return loaded = true;
}

std::string LevelMap::save() {
	using std::string;
	std::stringstream ss;
	
	// Level Data
	ss << "levelData = {\n";
	ss << "monetaryTarget = " << level_financial_target << ",\n";
	ss << "timeLimit = " << level_time_limit_s << "\n";
	ss << "}\n\n";
	
	// Player State
	ss << "playerState = {\n";
	ss << "balance = " << cash << "\n";
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
	Furnishing *f = new Furnishing(this, view__level, navmap, placeableMode);
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
	Furnishing *f = new Furnishing(this, view__level, navmap, false);
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
	Building *b = new Building(this, view__level, navmap);
	b->setType(type);
	b->setUp();
	b->setPos(pos);
	buildings.push_back(b);
	return b;
}
Building* LevelMap::createBuilding(LuaObj &o) {
	Building *b = new Building(this, view__level, navmap);
	b->deserialize(o);
	b->setUp();
	buildings.push_back(b);
	return b;
}

Unit* LevelMap::createUnit(bool placeableMode, const std::string &type, const W::position &pos) {
	Unit *u = new Unit(this, view__level, navmap, placeableMode);
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
	Unit *u = new Unit(this, view__level, navmap, false);
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
Controller* LevelMap::createController(const std::string &type, bool active) {
	Controller *c = NULL;
	if (type == "CustomerController") {
		c = new CustomerController(this, view__level, navmap, seekTarget__getRandom());
	}
	else if (type == "ShopkeeperController") {
		c = new ShopkeeperController(this, view__level, navmap);
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
		c = new CustomerController(this, view__level, navmap, seekTarget__getRandom());
	}
	else if (type == "ShopkeeperController") {
		c = new ShopkeeperController(this, view__level, navmap);
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
  const std::map<std::string, std::string> types = {
    {"customer", "CustomerController"},
    {"shopkeeper", "ShopkeeperController"},
  };

  auto it = types.find(u->type);
  if (it == types.end()) {
    throw W::Exception(std::string("Error: couldn't create controller for unit - unrecognised type: ") +
                       std::string("'") + u->type + std::string("'"));
  }
	
  return createController(it->second);
}

//void LevelMap::buyUnit(std::string type) {
//  int cost = Unit::cost_for_type(type);
//  bool bought = addPlayerMoneys(-cost);
//  if (bought) {
//    createUnit(true, type, {-1,-1});
//  }
//}

SeekTarget::Type LevelMap::seekTarget__getRandom() {
  return (SeekTarget::Type) W::Rand::intUpTo(SeekTarget::__N);
}

Building* LevelMap::building__getRandom() {
	if (buildings.size() == 0) {
		return NULL;
	}
	return (Building*) buildings[W::Rand::intUpTo((int) buildings.size())];
}
Building* LevelMap::building__findAt(W::position p) {
  auto it = std::find_if(buildings.begin(), buildings.end(), [=](TLO *b) {
    return ((Building*)b)->contains_point(p);
  });
  return it == buildings.end() ? NULL : (Building*)*it;
}
Building* LevelMap::building__withFurnishingSupportingSeekTarget(SeekTarget::Type seek_target) {
	// Find a furnishing supporting the SeekTarget
	tlovec supporting_furnishings;
	std::copy_if(furnishings.begin(), furnishings.end(), std::back_inserter(supporting_furnishings), [=](const TLO *x) {
		Furnishing *f = (Furnishing*) x;
		return f->supports_seekTarget(seek_target);
	});

	if (supporting_furnishings.size() == 0) {
		return NULL;
	}

  tlovec containing_buildings;
  std::transform(supporting_furnishings.begin(), supporting_furnishings.end(), std::back_inserter(containing_buildings), [=](const TLO *x) {
    return building__findAt(x->rct.pos);
  });
  if (containing_buildings.size() == 0) {
    return NULL;
  }

  auto last = std::unique(containing_buildings.begin(), containing_buildings.end());
  containing_buildings.erase(last, containing_buildings.end());

  return (Building*) containing_buildings[W::Rand::intUpTo((int) containing_buildings.size())];
}
std::vector<Furnishing*> LevelMap::furnishings__inBuilding__NotOwnedByController(Building *b) {
  std::vector<TLO*> out_tlos;
  std::copy_if(furnishings.begin(), furnishings.end(), std::back_inserter(out_tlos), [=](TLO *_f) {
    Furnishing *f = (Furnishing*) _f;
    if (f->owned_by_controller) {
      return false;
    }
    Building *b_pos = building__findAt(f->rct.pos);
    return b_pos == b;
  });

  std::vector<Furnishing*> out;
  std::transform(out_tlos.begin(), out_tlos.end(), std::back_inserter(out), [=](TLO *t) {
    return (Furnishing*) t;
  });

  return out;
}


W::position LevelMap::map__randomCoord() {
	return W::position(
		W::Rand::intUpTo(width()),
		W::Rand::intUpTo(height())
	);
}
Building* LevelMap::map__randomBuilding(std::string type) {
  tlovec vec;
  if (type != "") {
    std::copy_if(buildings.begin(), buildings.end(), std::back_inserter(vec), [=](TLO* _b) {
      Building *b = (Building*) _b;
      return b->type == type;
    });
  }
  else {
    vec = buildings;
  }

  if (vec.size() == 0) {
    return NULL;
  }
  return (Building*) vec[W::Rand::intUpTo((int)vec.size())];
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
	int result = cash + x;
	if (result < 0) {
		return false;
	}
	cash = result;
	return true;
}

void LevelMap::tlovec__update(tlovec &v) {
  for (auto it : v) {
		it->update();
	}
}
void LevelMap::tlovec__clearDestroyeds(tlovec &v) {
  tlovec out;

  std::copy_if(v.begin(), v.end(), std::back_inserter(out), [=](TLO *tlo) {
    bool destroy = tlo->destroyed;
    if (destroy) {
      delete tlo;
    }
    return !destroy;
  });

  v = out;
}
