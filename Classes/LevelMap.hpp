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

class State__Game;
class LevelView;
class View__BottomBar;
class TLO;
class Furnishing;
class Building;
class Unit;

class LevelMap {
public:
	LevelMap(State__Game*, LevelView*, View__BottomBar*);
	~LevelMap();
	
	void update(int frame_microseconds, float time_in_level);
	bool load(lua_State*);
	std::string save();
	
	W::EventPropagation::T keyEvent(W::Event*);
  W::EventPropagation::T buttonEvent(W::Event*);
  W::EventPropagation::T economicEvent(W::Event*);
	
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

  SeekTarget::Type seekTarget__getRandom();
	
	Building* building__getRandom();
	Building* building__findAt(W::position);
	Building* building__withFurnishingSupportingSeekTarget(SeekTarget::Type);
  std::vector<Furnishing*> furnishings__inBuilding__NotOwnedByController(Building*);
	
	W::position map__randomCoord();
	Building* map__randomBuilding(std::string type = "");
	
	void deactivateController(Controller *);
	void reactivateController(Controller *);
	
  int get_cash() { return cash; }
  bool can_afford(int cost) { return cash >= cost; }
  bool addPlayerMoneys(int);
	
private:
	State__Game *state__game;
	LevelView *view__level;
  View__BottomBar *view__btmBar;
	
	W::NavMap *navmap;
	
	// Game objects
	typedef std::vector<TLO*> tlovec;
	tlovec units;
	tlovec furnishings;
	tlovec controllers;
	tlovec buildings;
	
	tlovec controllers_to_deactivate;
	tlovec controllers_to_reactivate;
	tlovec inactiveControllers;

  bool first_frame = true;
	
	// Level goal data
	int level_financial_target;
	int level_time_limit_s;
	
	// Player state
	int cash;
	
	void tlovec__update(tlovec &);
	void tlovec__clearDestroyeds(tlovec &);
		// Delete & remove destroyed TLOs from vector.
		//  - It is the dying TLO's responsibility to inform any subscribers
		//    of its demise.
	
	// Map properties & loading
	bool loaded;
	W::size mapSize;
};

#endif
