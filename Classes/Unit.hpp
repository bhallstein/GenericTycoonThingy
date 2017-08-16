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
class Building;
class Level;

struct unitInfo {
	std::string col, hoverCol;
};

namespace Intention {
	enum Enum {
		HAIRCUT, PIE, DESTRUCT, NONE
	};
};

class Unit : public MappedObj {
public:
	Unit(ResponderMap *, NavMap *, const char *_type, Level *);
	~Unit();
	
	// Properties
	std::string type;
	int prev_x, prev_y;
	int dest_x, dest_y;
	Building *dest_building;
	Level *level;
	
	// Methods
	void receiveEvent(Event *);
	void update();
	bool canPlace(int _x, int _y);
	const char * col();
	
	void addIntention(Intention::Enum);
	static bool initialize(W *); 	// Populate static unitTypes from units.lua
	
protected:
	// Intention implementation
	void intentionUpdate_Haircut();
	void intentionUpdate_Pie();
	void intentionUpdate_Destruct();
	std::vector<Intention::Enum> intentions;
	int stage;
	void nextIntention();
	
	bool incrementLocation();		// Move along route. Returns false if an obstacle is encountered.
	inline bool atDest();
	inline bool inHinterland();

	
	// Properties
	std::string *u_colour;
	std::string *u_hoverColour;
	std::string *u_colourWhenMoving;
	int frames_waited;
	NavMap *navmap;
	std::vector<NavNode *> route;
	bool hover;
	
	// Static members
	static std::map<std::string, unitInfo> unitTypes;	// e.g. "civilian" => struct unitInfo { }
	static std::string defaultColour;
	static std::string defaultHoverColour;
};

#endif
