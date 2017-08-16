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
};

class Unit : public MappedObj {
public:
	Unit(NavMap *, int _x, int _y, const char *type);
	~Unit();
	
	// Properties
	std::string type;
	int dest_x, dest_y;
	
	// Methods
	void receiveEvent(Event *);
	const char * col();
	void update();
	
	static bool initialize(W *); 	// Populate static unitTypes from units.lua
	
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
	enum state_types { S_IDLE, S_TRAVELING, S_WAITING } state;
	int frames_waited;
	NavMap *navmap;
	std::vector<NavNode *> route;
	bool hover;
	std::string *u_colour;
	std::string *u_hoverColour;
	std::string *u_colourWhenMoving;
	
	// Static members
	static std::map<std::string, struct unitInfo> unitTypes;	// e.g. "civilian" => struct unitInfo { }
	static std::string defaultColour;
	static std::string defaultHoverColour;
	static std::string defaultColourWhenMoving;
};

#endif
