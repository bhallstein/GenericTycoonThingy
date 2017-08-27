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
#include "SpawnPoint.hpp"
#include "Unit.hpp"
#include "Furnishing.hpp"
#include "Building.hpp"

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

void LevelMap::update() {
	updateTLOVec(spawnPoints);
	updateTLOVec(units);
	updateTLOVec(furnishings);
	updateTLOVec(controllers);
	updateTLOVec(buildings);
}

W::EventPropagation::T LevelMap::keyEvent(W::Event *ev) {
	if (ev->key == W::KeyCode::_U) createUnit(true, "customer", W::position());
	else if (ev->key == W::KeyCode::_F) createFurnishing(true, "barberschair", W::position());
	return W::EventPropagation::ShouldContinue;
}

bool LevelMap::load(LuaObj &mapData) {
	if (loaded) return false;
	
	// Note: LevelState::loadLevel() has already checked mapData is a table.
	
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
	for (LuaObj::_descendantmap::iterator it = d->begin(); it != d->end(); ++it)
		createFurnishing(it->second);
	
	// Get units
	LuaObj &unitsObj = mapData["units"];
	if (!unitsObj.isTable()) {
		W::log << "mapData.units table not found" << std::endl;
		return false;
	}
	d = &unitsObj.descendants;
	for (LuaObj::_descendantmap::iterator it = d->begin(); it != d->end(); ++it)
		createUnit(it->second);
	
	// Get spawn points
	LuaObj &spawnPointsObj = mapData["spawnPoints"];
	if (!spawnPointsObj.isTable()) {
		W::log << "mapData.spawnPoints table not found" << std::endl;
		return false;
	}
	d = &spawnPointsObj.descendants;
	for (LuaObj::_descendantmap::iterator it = d->begin(); it != d->end(); ++it)
		createSpawnPoint(it->second);
	
	// Get controllers
	LuaObj &controllersObj = mapData["controllers"];
	d = &controllersObj.descendants;
	for (LuaObj::_descendantmap::iterator it = d->begin(); it != d->end(); ++it)
		createController(it->second, true);
	
	// Get inactive controllers
	LuaObj &inactiveControllersObj = mapData["inactiveControllers"];
	d = &inactiveControllersObj.descendants;
	for (LuaObj::_descendantmap::iterator it = d->begin(); it != d->end(); ++it)
		createController(it->second, false);
	
	// Get buildings
	LuaObj &buildingsObj = mapData["buildings"];
	if (buildingsObj.isTable()) {
		d = &buildingsObj.descendants;
		for (LuaObj::_descendantmap::iterator it = d->begin(); it != d->end(); ++it)
			createBuilding(it->second);
	}
	
	return loaded = true;
}

std::string LevelMap::save() {
	using std::string;
	std::stringstream ss;
	
	ss << "width = " << mapSize.width << ",\n";
	ss << "height = " << mapSize.height << ",\n\n";
	
	// Units
	ss << "units = {\n";
	if (units.size() == 0) ss << "\n";
	for (tloVec::iterator it = units.begin(), itend = units.end(); it < itend; ++it)
		ss << "{\n" << (*it)->serialize() << (it == itend-1 ? "}\n" : "},\n");
	ss << "},\n";
	
	// Furnishings
	ss << "furnishings = {\n";
	if (furnishings.size() == 0) ss << "\n";
	for (tloVec::iterator it = furnishings.begin(), itend = furnishings.end(); it < itend; ++it)
		ss << "{\n" << (*it)->serialize() << (it == itend-1 ? "}\n" : "},\n");
	ss << "},\n";
	
	// SPs
	ss << "spawnPoints = {\n";
	if (spawnPoints.size() == 0) ss << "\n";
	for (tloVec::iterator it = spawnPoints.begin(), itend = spawnPoints.end(); it < itend; ++it)
		ss << "{\n" << (*it)->serialize() << (it == itend-1 ? "}\n" : "},\n");
	ss << "},\n";
	
	// Controllers
	ss << "controllers = {\n";
	if (controllers.size() == 0) ss << "\n";
	for (tloVec::iterator it = controllers.begin(), itend = controllers.end(); it < itend; ++it)
		ss << "{\n" << (*it)->serialize() << (it == itend-1 ? "}\n" : "},\n");
	ss << "},\n";
	
	// Inactive Controllers
	ss << "inactiveControllers = {\n";
	if (inactiveControllers.size() == 0) ss << "\n";
	for (tloVec::iterator it = inactiveControllers.begin(), itend = inactiveControllers.end(); it < itend; ++it)
		ss << "{\n" << (*it)->serialize() << (it == itend-1 ? "}\n" : "},\n");
	ss << "}\n";
	
	return ss.str();
}

Furnishing* LevelMap::createFurnishing(bool placeableMode, const std::string &type, const W::position &pos) {
	Furnishing *f = new Furnishing(levelState, this, levelView, navmap, placeableMode);
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
	Furnishing *f = new Furnishing(levelState, this, levelView, navmap, false);
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
	Building *b = new Building(levelState, this, levelView, navmap);
	b->setType(type);
	b->setUp();
	b->setPos(pos);
	return b;
}
Building* LevelMap::createBuilding(LuaObj &o) {
	Building *b = new Building(levelState, this, levelView, navmap);
	b->deserialize(o);
	b->setUp();
	buildings.push_back(b);
	return b;
}

Unit* LevelMap::createUnit(bool placeableMode, const std::string &type, const W::position &pos) {
	Unit *u = new Unit(levelState, this, levelView, navmap, placeableMode);
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
	Unit *u = new Unit(levelState, this, levelView, navmap, false);
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
SpawnPoint* LevelMap::createSpawnPoint(bool placeableMode, const W::position &pos) {
	SpawnPoint *sp = new SpawnPoint(levelState, this, levelView, navmap, placeableMode);
	sp->setUp();
	bool initsuccess = sp->init(placeableMode ? W::position(-100,-100) : pos);
	if (initsuccess) {
		spawnPoints.push_back(sp);
		W::log << "SpawnPoint " << sp->uid.id << " created and initialized "
			<< (placeableMode ? "in placeable mode" : std::string("at ") + sp->rct.pos.xyStr()) << std::endl;
	}
	else {
		delete sp;
		sp = NULL;
	}
	return sp;
}
SpawnPoint* LevelMap::createSpawnPoint(LuaObj &o) {
	SpawnPoint *sp = new SpawnPoint(levelState, this, levelView, navmap, false);
	sp->deserialize(o);
	sp->setUp();
	if (sp->init()) {
		spawnPoints.push_back(sp);
		W::log << "SpawnPoint " << sp->uid.id << "deserialized and initialized at " << sp->rct.pos.xyStr() << std::endl;
	}
	else {
		W::log << "SpawnPoint " << sp->uid.id << " deserialized then failed to initialize at " << sp->rct.pos.xyStr() << std::endl;
		delete sp;
		sp = NULL;
	}
	return sp;
}
Controller* LevelMap::createController(const std::string &type, bool active) {
	Controller *c = NULL;
	if (type == "CustomerController") {
		c = new CustomerController(levelState, this, levelView, navmap);
	}
	if (c != NULL) {
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
		c = new CustomerController(levelState, this, levelView, navmap);
		c->deserialize(o);
	}
	if (c != NULL) {
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
	else if (type == "blah") { } // ...
	
	if (c == NULL) throw W::Exception(
		string("Error: couldn't create unit behaviour - unrecognised type ") +
		string("'") + type + string("'")
	);
	
	return c;
}

void LevelMap::deactivateController(Controller *c) {
	inactiveControllers.push_back(c);
	for (tloVec::iterator it = controllers.begin(); it < controllers.end(); )
		if (*it == c) it = controllers.erase(it);
		else ++it;
}
void LevelMap::reactivateController(Controller *c) {
	controllers.push_back(c);
	for (tloVec::iterator it = inactiveControllers.begin(); it < inactiveControllers.end(); )
		if (*it == c) it = inactiveControllers.erase(it);
		else ++it;
}

void LevelMap::updateTLOVec(std::vector<TLO *> &v) {
	for (tloVec::iterator it = v.begin(); it < v.end(); ) {
		TLO *tlo = *it;
		tlo->update();
		if (tlo->destroyed) {
			delete tlo;
			it = v.erase(it);
		}
		else ++it;
	}
}
