/* 
 * Unit.hpp
 *
 */

#ifndef UNIT_H
#define UNIT_H

#include <iostream>
#include <map>

#include "types.hpp"
#include "PlaceableManager.hpp"
#include "Behaviour.hpp"

namespace W { namespace EventType {
	extern T INTERRUPT_UNITPICKUP;
} }


struct unitInfo {
	unitInfo(LuaObj &);
	bool isStaff;
	int hireCost;
};


class Building;
class LevelState;
class LevelView;
class Furnishing;

class Unit : public PlaceableManager {
public:
	Unit(LevelState *, LevelMap *, LevelView *, W::NavMap *, bool _placeableMode);
	~Unit();
		// When a unit is constructed, we may not yet know its type, which is therefore
		// initially set to NO_TYPE.
		// The unit must then be "set up".
		//   setType(t)	- set the type manually
		//				- (not req’d if being deserialized)
		//   setUp() - perform setup (after type has been set)
		
		// Note: then need to init() the Unit, since it is a PlaceableManager.
	
	void setUp();
	
	void mouseEvent(W::Event *);
	void update();
	
	// Utility methods
	void wanderToRandomMapDestination();
	void getDespawnPoint(W::position &);
	bool voyage(const W::position &_dest);
	
	static bool initialize(); 	// Populate static unitTypes from units.lua
	static bool initialized;
	
	class DrawnUnit;
	
protected:
	UnitMode::T mode;
	Behaviour *behaviour;
	
	// PlaceableManager overrides
	void placementLoopStarted();
	void placementLoopUpdate();
	void placementLoopCancelled();
	void placementLoopSucceeded();
	bool canPlace(const W::position &);
	
	W::position dest;
	bool incrementLocation();	// Move along route. Returns false if obstacle encountered.
	inline bool inHinterland();
	
	DrawnUnit *drawnUnit;
	struct unitInfo *typeInfo;
	
	bool hired;
	
	std::vector<W::position> route;
	
	// Serialization
	static serialization_descriptor sd;
	sdvec _getSDs() {
		sdvec vec;
		vec.push_back(&Unit::sd);
		return vec;
	}
	void deserializeAdditionalProperties(LuaObj &o) {
		createBehaviour();
		behaviour->deserialize(o["behaviour"]);
	}
	void getAdditionalSerializedProperties(std::map<std::string, std::string> &m) {
		m["behaviour"] = behaviour->serialize();
	}
	
	static std::map<std::string, unitInfo*> unitTypeInfo;
	
	void createBehaviour() {
		using std::string;
		if (type == "customer")
			behaviour = new CustomerBehaviour(this);
		else
			throw W::Exception(
				string("Error: couldn't create unit behaviour - unrecognised type '") +
				type + string("'")
			);
	}
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
	W::DrawnRect *r;
};

#endif
