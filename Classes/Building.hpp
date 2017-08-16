/*
 * Building.h - a building
 *
 */

#ifndef BUILDING_H
#define BUILDING_H

#include <iostream>

#include "types.h"
#include "MappedObj.hpp"

class Building : public MappedObj {
public:
	Building(int _x, int _y);
	~Building();
	
	// Properties
	enum BuildingType { DERELICT, HOME, BARBER, PIESHOP } type;
	bool destroyed;
	std::vector<door> doors;		// Doors. These should probably be on the edge of the building. lol.
									// See types.h for what a door looks like.
	// Methods
	void receiveEvent(Event *ev);	// Handle mouse events
	void update() { }
	colour_name col();				// Temporary coloration-signifier

protected:
	// Properties
	bool clicked;
	int time_hovered;

};

#endif
