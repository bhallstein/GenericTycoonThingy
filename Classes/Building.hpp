/*
 * Generic Tycoon Thingy
 *
 * ================
 *  Building.hpp
 * ================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

/*
 * Building.hpp
 * 
 * Building is a TLO subclass, since Buildings are saved in levels
 * and hence need to be serializable.
 *
 */

#ifndef BUILDING_H
#define BUILDING_H

#include <iostream>
#include <map>
#include <string>
#include <queue>

#include "types.hpp"
#include "TLO.hpp"

class LevelState;
class Unit;
class Furnishing;

struct buildingInfo {
	buildingInfo(LuaObj &);
	std::vector<std::string> allowedFurnishings;
};

class Building : public TLO {
public:
	Building(LevelState *, LevelMap *, LevelView *, W::NavMap *);
	~Building();
	void _setUp();
		// See note in Unit.hpp on creation & setup
	
	void update() { }
	
	static bool initialize();
	static bool initialized;
	
	void setPos(const W::position &);
	
protected:
	// Serialization
//	virtual void getSDs(sdvec &vec) {
//		TLO::getSDs(vec);
//		vec.push_back(&Building::sd);
//	}
//	static serialization_descriptor sd;
	
	// Type info
	struct buildingInfo *typeInfo;
	static std::map<std::string, buildingInfo*> buildingTypeInfo;
	
	// Drawing
	class DrawnBuilding;
	DrawnBuilding *drawnBuilding;
};


class Building::DrawnBuilding {
public:
	DrawnBuilding(LevelView *, const W::position &);
	~DrawnBuilding();
	void setPosn(const W::position &);
//	void setOpac(float);
private:
	W::DRect *r;
	LevelView *lv;
};

#endif
