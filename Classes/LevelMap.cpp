#include "LevelMap.hpp"
#include "LevelView.hpp"
#include "TLO.hpp"
#include "LuaObj.h"
#include "SpawnPoint.hpp"


LevelMap::LevelMap(LevelState *_ls, LevelView *_lv) :
	levelState(_ls),
	levelView(_lv),
	loaded(false)
{
	W::Messenger::subscribeToEventType(W::EventType::KEYUP, W::Callback(&LevelMap::keyEvent, this));
}
LevelMap::~LevelMap()
{
	// bai
}

void LevelMap::update() {
	updateTLOVec(units);
	updateTLOVec(furnishings);
}

W::EventPropagation::T LevelMap::keyEvent(W::Event *ev) {
	if (ev->key == W::KeyCode::_U) createUnit(true, "customer");
	else if (ev->key == W::KeyCode::_F) createFurnishing(true, "barberschair");
	return W::EventPropagation::SHOULD_CONTINUE;
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
	for (tloVec::iterator it = units.begin(), itend = units.end(); it < units.end(); ++it)
		ss << "{\n" << (*it)->serialize() << (it == itend-1 ? "}\n" : "},\n");
	ss << "},\n";
	
	// Furnishings
	ss << "furnishings = {\n";
	if (furnishings.size() == 0) ss << "\n";
	for (tloVec::iterator it = furnishings.begin(), itend = furnishings.end(); it < furnishings.end(); ++it)
		ss << "{\n" << (*it)->serialize() << (it == itend-1 ? "}\n" : "},\n");
	ss << "},\n";
	
	// SPs
	ss << "spawnPoints = {\n";
	if (spawnPoints.size() == 0) ss << "\n";
	for (tloVec::iterator it = spawnPoints.begin(), itend = spawnPoints.end(); it < spawnPoints.end(); ++it)
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
		f->uid = UIDManager::getNewUID();
		UIDManager::registerTLO(f);
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
		UIDManager::registerTLO(f);
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
Unit* LevelMap::createUnit(bool placeableMode, const std::string &type, const W::position &pos) {
	Unit *u = new Unit(levelState, this, levelView, navmap, placeableMode);
	u->setType(type);
	u->setUp();
	bool initsuccess = u->init(placeableMode ? W::position(-100,-100) : pos);
	if (initsuccess) {
		u->uid = UIDManager::getNewUID();
		UIDManager::registerTLO(u);
		units.push_back(u);
		W::log << "Unit " << u->uid.id << " created and initialized "
			<< (placeableMode ? "in placeable mode" : std::string("at ") + u->rct.pos.xyStr()) << std::endl;
	}
	else {
		W::log << "Unit " << u->uid.id << " created then failed to initialize" << std::endl;
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
		UIDManager::registerTLO(u);
		units.push_back(u);
		W::log << "Unit " << u->uid.id << " deserialized and initialized at " << u->rct.pos.xyStr() << std::endl;
	}
	else {
		W::log << "Unit " << u->uid.id << " deserialized then failed to initialize at " << u->rct.pos.xyStr() << std::endl;
		delete u;
		u = NULL;
	}
	return u;
}
SpawnPoint* LevelMap::createSpawnPoint(bool placeableMode, const W::position &pos) {
	SpawnPoint *sp = new SpawnPoint(levelState, this, levelView, navmap, placeableMode);
	bool initsuccess = sp->init(placeableMode ? W::position(-100,-100) : pos);
	if (initsuccess) {
		sp->uid = UIDManager::getNewUID();
		UIDManager::registerTLO(sp);
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
	if (sp->init()) {
		UIDManager::registerTLO(sp);
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

void LevelMap::destroyTLO(TLO *tlo) {
	UIDManager::unregisterTLO(tlo);
	delete tlo;
}

void LevelMap::updateTLOVec(std::vector<TLO *> &v) {
	for (tloVec::iterator it = v.begin(); it < v.end(); ) {
		TLO *tlo = *it;
		tlo->update();
		if (tlo->destroyed) {
			destroyTLO(tlo);
			it = v.erase(it);
		}
		else ++it;
	}
}
