/*
 * Building.hpp
 * 
 * Buildings are slightly different from the other TLOs. They aren’t really
 * first-class game objects in their own right, but just loose collections
 * of other objects on the map, for convenience.
 *
 * Therefore, a Building does not “own” anything, nor is it updated. It is
 * just a receptacle for other things -- a helper class for letting LevelMap
 * discern the rules by which the “true” top-level objects interact.
 * 
 * Building is, however, a TLO subclass, since Buildings are saved in levels
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
	Building(LevelState *, LevelMap *, LevelView *, W::NavMap *, const char *_type, std::vector<W::rect> *_plan, W::position &);
	~Building();
	
	// Properties
	std::string type;
	struct buildingInfo *bInfo;
	
	// Methods
	W::EventPropagation::T mouseEvent(W::Event *);
	void update() { }
	
	static bool initialize();
	
protected:
	
	static std::map<std::string, buildingInfo*> buildingTypeInfo;
	static bool initialized;
	
	static std::vector<std::map<Furnishing*, Unit*>::iterator> _ind_array;
};

#endif
