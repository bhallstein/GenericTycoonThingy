/*
 * Furniture.hpp
 * 
 */

#ifndef FURNITURE_H
#define FURNITURE_H

#include <iostream>

#include "types.hpp"
#include "MappedObj.hpp"

class ResponderMap;
class NavMap;
class W;
class Building;

struct furnitureInfo {
	std::string col, hoverCol;
	std::vector<intcoord> groundplan;
};

class Furniture : public MappedObj {
public:
	Furniture(ResponderMap *, NavMap *, const char *_type, Building *);
	~Furniture();
	
	// Properties
	std::string type;
	
	// Methods
	void receiveEvent(Event *);
	void update() { }
	bool canPlace(int _x, int _y);
	void finalizePlacement();
	const char * col();
	
	static bool initialize(W *);	// Populate our static furnitureTypes map from furniture.lua

protected:
	std::string *f_colour;
	std::string *f_hoverColour;
	
	NavMap *navmap;
	Building *contextBuilding;
	
	// Static members
	static std::map<std::string, struct furnitureInfo> furnitureTypes;	// e.g. "barber's chair" => struct unitInfo { }
	static std::string defaultColour;
	static std::string defaultHoverColour;
};

#endif
