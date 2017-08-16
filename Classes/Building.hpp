/*
 * Building.h - a building
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
class LuaHelper;

struct buildingInfo {
	std::string col, hoverCol;
	//allowedPlaceables;
};

class Building : public MappedObj {
public:
	Building(int _x, int _y, const char *type);
	~Building();
	
	// Properties
	std::string type;
	bool destroyed;
	std::vector<door> doors;		// Doors. These should probably be on the edge of the building. lol.
									// See types.hpp for what a door looks like
	// Methods
	void receiveEvent(Event *);		// Handle mouse events
	void update() { }
	const char * col();
	
	static bool initialize(W *);	// Populate our static buildingTypes map from buildings.lua
	
protected:
	// Properties
	bool clicked;
	int time_hovered;
	std::string b_colour;
	std::string b_hoverColour;
	
	// Info on building types, saved as static members for private use by Building & its instances.
	static std::map<std::string, struct buildingInfo> buildingTypes;	// e.g. "pieshop" => struct buildingInfo { }
	static std::string defaultColour;
	static std::string defaultHoverColour;
};

#endif
