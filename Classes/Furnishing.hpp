/*
 * Generic Tycoon Thingy
 *
 * =================
 *  Furnishing.hpp
 * =================
 *
 * Copyright (C) 2012 - Ben Hallstein
 * All rights reserved
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
	std::vector<W::iRect> plan;
	std::vector<SeekTarget::Type> seekables;
};

class Furnishing : public PlaceableManager {
public:
	Furnishing(LevelMap *, View__Game *, W::NavMap *, bool _placeableMode);
	~Furnishing();
	void _setUp();
		// See note in Unit.hpp on creation & setup
	
	// Event callbacks
	W::EventPropagation::T mouseEvent(W::Event *);

	void update();
	
	static bool initialize();
	static bool initialized;

  static std::vector<std::string> get_furnishing_types();
	static int costForType(const char *);
	bool supports_seekTarget(SeekTarget::Type) const;

  bool owned_by_controller;
	
protected:
	bool purchased;
	
	// PlaceableManager overrides
	void placementLoopStarted();
	void placementLoopUpdate();
	void placementLoopCancelled();
	void placementLoopSucceeded();
	bool canPlace(W::v2f);
  bool placed;
	
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
	class DrawnFurnishing;
	DrawnFurnishing *drawnFurnishing;
};


class Furnishing::DrawnFurnishing {
public:
	DrawnFurnishing(View__Game *);
	~DrawnFurnishing();
	void setPosn(W::v2f);
	void setOpac(float);
	void setColour(W::Colour);
private:
	View__Game *lv;
	W::Rectangle *r;
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
