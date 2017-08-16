/*
 * Placeable.h
 *
 */

#ifndef PLACEABLE_H
#define PLACEABLE_H

#include <iostream>
#include <map>

#include "types.hpp"
#include "MappedObj.hpp"

class NavMap;
class ResponderMap;
class W;
class Building;

struct placeableInfo {
	std::string col, hoverCol, colWhilePlacing;
	std::vector<intcoord> groundplan;
};

class Placeable : public MappedObj {
public:
	Placeable(NavMap *, ResponderMap *, const char *_type, Building *_context);
	~Placeable();
	
	// Methods
	void receiveEvent(Event *ev);
	void update() { }
	const char * col();
	
	static bool initialize(W *);	// Populate our static buildingTypes map from buildings.lua

protected:
	// Properties
	enum mode_types { PLACEMENT, PLACED } mode;
	NavMap *navmap;
	ResponderMap *levelResponderMap;
	Building *contextBuilding;
	std::string type;
	std::string *p_colour;
	std::string *p_hoverColour;
	std::string *p_colourWhilePlacing;
	
	// Static members
	static std::map<std::string, struct placeableInfo> placeableTypes;	// e.g. "barber's chair" => struct unitInfo { }
	static std::string defaultColour;
	static std::string defaultHoverColour;
	static std::string defaultColourWhilePlacing;
};

#endif
