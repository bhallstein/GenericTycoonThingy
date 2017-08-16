/*
 * Building.h - a building
 *
 */

#ifndef BUILDING_H
#define BUILDING_H

#include <iostream>

#include "types.hpp"
#include "MappedObj.hpp"

class Building : public MappedObj {
public:
	Building(); //for templates only!
	Building(int _x, int _y);
	~Building();

	// Properties
	colour defaultCol;
	std::string type;
	bool destroyed;
	std::vector<door> doors;		// Doors. These should probably be on the edge of the building. lol.
									// See types.hpp for what a door looks like.
	// Methods
	void receiveEvent(Event *ev);	// Handle mouse events
	void update() { }
	colour col();
	
protected:
	// Properties
	bool clicked;
	int time_hovered;
	
};

#endif