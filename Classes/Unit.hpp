/* 
 * Unit.hpp
 *
 */

#ifndef UNIT_H
#define UNIT_H

#include <iostream>
#include <map>

#include "types.hpp"
#include "MappedObj.hpp"
#include "../W.hpp"

class NavMap;
class NavNode;

struct unitInfo {
	std::string col, hoverCol, colWhenMoving;
	int hireCost;
};

class Unit : public MappedObj {
public:
	Unit(ResponderMap *, NavMap *, const char *_type);
	~Unit();
	
	// Properties
	std::string type;
	int dest_x, dest_y;
	
	// Methods
	void receiveEvent(Event *);
	void update();
	bool canPlace(int _x, int _y);
	const char * col();
	
	static bool initialize(W *); 	// Populate static unitTypes from units.lua

	//UnitInfo gets
	static int getUnitHireCost(std::string); //lookup a unitInfo hireCost from unitTypes
protected:
	// Methods
	void nextInRoute();
	void setToIdle();
	void setToTraveling();
	void setToWaiting();
	void incrementLocation();
	void seekHinterland();
	bool inHinterland();
	
	// Properties
	std::string *u_colour;
	std::string *u_hoverColour;
	std::string *u_colourWhenMoving;
	enum state_types { S_IDLE, S_TRAVELING, S_WAITING } state;
	int frames_waited;
	NavMap *navmap;
	std::vector<NavNode *> route;
	bool hover;
	
	// Static members
	static std::map<std::string, struct unitInfo> unitTypes;	// e.g. "civilian" => struct unitInfo { }
	static std::string defaultColour;
	static std::string defaultHoverColour;
	static std::string defaultColourWhenMoving;
};

#endif
