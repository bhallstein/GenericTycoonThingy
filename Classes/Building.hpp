/*
 * Building.hpp
 *
 */

#ifndef BUILDING_H
#define BUILDING_H

#include <iostream>
#include <map>
#include <string>

#include "types.hpp"
#include "MappedObj.hpp"

class W;
class Level;
class LuaHelper;
class NavMap;
class Unit;
class Furnishing;

struct buildingInfo {
	std::string col, hoverCol;
	std::vector<std::string> allowedFurnishings;
};

class Building : public MappedObj {
public:
	Building(ResponderMap *, NavMap *, const char *_type, std::vector<intcoord> *_groundplan, std::vector<door> *_doors, Level *);
	~Building();
	
	// Properties
	std::string type;
	std::vector<door> doors;		// Doors. These should probably be on the edge of the building. lol.
	std::string *b_colour;
	std::string *b_hoverColour;
	std::vector<std::string> *b_allowedFurnishings;
									// See types.hpp for what a door looks like
	// Methods
	void receiveEvent(Event *);		// Handle mouse events
	void update();
	bool canPlace(int _x, int _y);
	void finalizePlacement();
	const char * col();
	
	void getEntryPoint(int *_x, int *_y);
	void getQueuePoint(int *_x, int *_y);
	void addToQueue(Unit *);
	void addFurnishing(Furnishing *);
	void removeFurnishing(Furnishing *);
	void addStaff(Unit *);
	
	static bool initialize(W *);	// Populate our static buildingTypes map from buildings.lua
	static bool initialized;
	
protected:
	// Properties
	bool clicked;
	int time_hovered;
	NavMap *navmap;
	Level *level;
	// Dispatchery
	std::vector<Unit*> Q;
	std::vector<Unit*> staff;
	std::vector<Furnishing*> furnishings;
	std::map<Furnishing*, Unit*> staffBindings;
	
	// Info on building types, saved as static members for private use by Building & its instances.
	static std::map<std::string, struct buildingInfo> buildingTypes;	// e.g. "pieshop" => struct buildingInfo { }
	static std::string defaultColour;
	static std::string defaultHoverColour;
	
	static int *ind_array;
};

#endif
