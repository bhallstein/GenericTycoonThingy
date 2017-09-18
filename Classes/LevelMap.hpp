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
class Furnishing;
class Building;
class Unit;


class LevelMap {
public:
	LevelMap(LevelState *, LevelView *);
	~LevelMap();
	
	void update(int frame_microseconds, float time_in_level);
	bool load(lua_State*);
	std::string save();
	
	W::EventPropagation::T keyEvent(W::Event *);
	
	int width() { return mapSize.width; }
	int height() { return mapSize.height; }
	
	Furnishing* createFurnishing(bool placeableMode, const std::string &type, const W::position &pos);
	Furnishing* createFurnishing(LuaObj &);
	Building* createBuilding(const std::string &type, const W::position &pos);
	Building* createBuilding(LuaObj &);
	Unit* createUnit(bool placeableMode, const std::string &type, const W::position &pos);
	Unit* createUnit(LuaObj &);
	Controller* createController(const std::string &type, bool active = true);
	Controller* createController(LuaObj &, bool active = true);
	Controller* createControllerForUnit(Unit *);
	
	Building* building__getRandom();
	Building* building__findAt(W::position &);
	
	W::position map__randomCoord();
	
	void deactivateController(Controller *);
	void reactivateController(Controller *);
	
	bool addPlayerMoneys(int);
	
private:
	LevelState *levelState;
	LevelView *levelView;
	
	W::NavMap *navmap;
	
	// Game objects
	typedef std::vector<TLO*> tlovec;
	tlovec units;
	tlovec furnishings;
	tlovec controllers;
	tlovec buildings;
	tlovec inactiveControllers;
	
	// Level goal data
	int monetaryTarget;
	int timeLimit;
	
	// Player state
	int playerMoneys;
	
	void updateTLOVec(tlovec &);
		// Calls update() on each TLO.
		//  - After update(), if the TLO is 'destroyed', destroy it.
		//  - It is the dying TLO's responsibility to inform any subscribers
		//    of its demise.
	
	// Map properties & loading
	bool loaded;
	W::size mapSize;
};

#endif
