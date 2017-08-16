/*
 * Generic Tycoon Thingy
 *
 * ================
 *  LevelMap.hpp
 * ================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#ifndef __DBT__LevelMap__
#define __DBT__LevelMap__

#include <string>
#include <vector>

#include "W.h"
#include "LuaObj.h"
#include "Controller.hpp"

class LevelState;
class LevelView;
class TLO;
class Unit;
class Furnishing;
class SpawnPoint;

class LevelMap {
public:
	LevelMap(LevelState *, LevelView *);
	~LevelMap();
	
	void update();
	bool load(LuaObj &mapObj);
	std::string save();
	
	W::EventPropagation::T keyEvent(W::Event *);
	
	int width() { return mapSize.width; }
	int height() { return mapSize.height; }
	
	Furnishing* createFurnishing(bool placeableMode, const std::string &type, const W::position &pos = W::position());
	Furnishing* createFurnishing(LuaObj &);
	Unit* createUnit(bool placeableMode, const std::string &type, const W::position &pos = W::position());
	Unit* createUnit(LuaObj &);
	SpawnPoint* createSpawnPoint(bool placeableMode, const W::position &pos = W::position());
	SpawnPoint* createSpawnPoint(LuaObj &);
	Controller* createController(const std::string &type, bool active = true);
	Controller* createController(LuaObj &, bool active = true);
	Controller* createControllerForUnit(Unit *);
	
	void deactivateController(Controller *);
	void reactivateController(Controller *);
	
private:
	LevelState *levelState;
	LevelView *levelView;
	
	W::NavMap *navmap;
	
	typedef std::vector<TLO*> tloVec;
	tloVec units;
	tloVec furnishings;
	tloVec spawnPoints;
	tloVec controllers;
	tloVec inactiveControllers;
	
	void updateTLOVec(tloVec &);
		// Calls update() on each TLO.
		//  - After update(), if the TLO is 'destroyed', destroy it.
		//  - It is the dying TLO's responsibility to inform any subscribers
		//    of its demise.
	
	// Map properties & loading
	bool loaded;
	W::size mapSize;
};

#endif
