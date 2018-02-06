/*
 * Generic Tycoon Thingy
 *
 * ============
 *  Unit.hpp
 * ============
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#ifndef UNIT_H
#define UNIT_H

#include <iostream>
#include <map>

#include "types.hpp"
#include "PlaceableManager.hpp"

namespace W {
  namespace EventType {
    extern T INTERRUPT_UNITPICKUP;
  }
}

struct unitInfo {
	unitInfo(LuaObj &);
	bool isStaff;
	int hireCost;
};

class LevelState;
class LevelView;
class Furnishing;
class Controller;

class Unit : public PlaceableManager {
public:
	Unit(LevelMap *, LevelView *, W::NavMap *, bool _placeableMode);
	~Unit();
	void _setUp();
		// When a unit is constructed, we may not yet know its type, which is therefore
		// initially set to NO_TYPE.
		// The unit must then be "set up".
		//   setType(t)	- set the type manually
		//				- (not req’d if being deserialized)
		//   setUp() - perform setup (after type has been set)
		
		// Note: then also need to init() the Unit, since it is a PlaceableManager.
	
	// Event callbacks
	W::EventPropagation::T mouseEvent(W::Event *);
	
	void update();
	
	// Utility methods
	bool wanderToRandomMapDestination();
	bool voyage(const W::position &_dest);
	
	static bool initialize(); 	// Populate static unitTypes from units.lua
	static bool initialized;
	
	// Unit state properties: regrettably, a big-ass list for now
	// ...
	
	UID controller;
	
protected:
	UnitMode::T mode;
	Controller* controllerPtr() { return (Controller*) controller.get(); }
	bool hired;
	
	// PlaceableManager overrides
	void placementLoopStarted();
	void placementLoopUpdate();
	void placementLoopCancelled();
	void placementLoopSucceeded();
	bool canPlace(const W::position &);
	
	// Voyaging-related
	W::position dest;
	std::vector<W::position> route;
	bool incrementLocation();	// Move along route. Returns false if obstacle encountered.
	bool inHinterland();
	
	// Serialization
	static serialization_descriptor sd;
	virtual void getSDs(sdvec &vec) {
		PlaceableManager::getSDs(vec);
		vec.push_back(&Unit::sd);
	}
	void deserializeAdditionalProperties(LuaObj &o) { }
	void getAdditionalSerializedProperties(std::map<std::string, std::string> &m) { }
	
	// Type info
	struct unitInfo *typeInfo;
	static std::map<std::string, unitInfo*> unitTypeInfo;
	
	// Drawing
	class DrawnUnit;
	DrawnUnit *drawnUnit;
	
	// Other
	void printDebugInfo();
};


class Unit::DrawnUnit {
public:
	DrawnUnit(LevelView *);
	~DrawnUnit();
	void setPosn(const W::position &);
	void setOpac(float x);
	void incRot();
	
private:
	LevelView *lv;
	W::DRect *r;
};

#endif
