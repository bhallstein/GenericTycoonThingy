/*
 * Furnishing.hpp
 * 
 */

#ifndef FURNITURE_H
#define FURNITURE_H

#include <iostream>

#include "types.hpp"
#include "PlaceableManager.hpp"

class Building;
class Unit;

struct furnishingInfo {
	furnishingInfo(LuaObj &);
	int cost;
	std::vector<W::rect> plan;
};

class Furnishing : public PlaceableManager {
public:
	Furnishing(LevelState *, LevelMap *, LevelView *, W::NavMap *, bool _placeableMode);
	~Furnishing();
	void _setUp();
		// See note in Unit.hpp on creation & setup
	
	// Event callbacks
	W::EventPropagation::T mouseEvent(W::Event *);

	void update();
	
	static bool initialize();
	static bool initialized;
	
	static int costForType(const char *);
	
	class DrawnFurnishing;
	
protected:
	bool purchased;
	
	// PlaceableManager overrides
	void placementLoopStarted();
	void placementLoopUpdate();
	void placementLoopCancelled();
	void placementLoopSucceeded();
	bool canPlace(const W::position &);
	
	// Serialization
	virtual void getSDs(sdvec &vec) {
		PlaceableManager::getSDs(vec);
		vec.push_back(&Furnishing::sd);
	}
	static serialization_descriptor sd;
	
	// Type info
	struct furnishingInfo *typeInfo;
	static std::map<std::string, furnishingInfo*> furnishingTypeInfo;
	
	// Drawing
	DrawnFurnishing *drawnFurnishing;
};


class Furnishing::DrawnFurnishing {
public:
	DrawnFurnishing(LevelView *);
	~DrawnFurnishing();
	void setPosn(const W::position &);
	void setOpac(float);
private:
	LevelView *lv;
	W::DRect *r;
};


//class Door : public Furnishing {
//public:
//	Door(W::NavMap *, const char *_type, Level *, bool _placeableMode);
//	~Door();
//	
//	bool canPlace(int _x, int _y);
//	void finalizePlacement();
//};

#endif
