/*
 * Building.h - a building
 *
 */

#ifndef BUILDING_H
#define BUILDING_H

#include <iostream>

#include "types.h"
#include "MappedObj.hpp"

enum BuildingType { DERELICT, HOME, BARBER, PIESHOP };

class Building : public MappedObj
{
public:
	Building(int _x, int _y);
	~Building();

	// Methods
	void receiveEvent(Event *ev);	// Handle mouse events
	char col();						// Temporary coloration-signifier

	// Properties
	BuildingType type;
	bool destroyed;
	std::vector<door> doors;		// Doors. These should probably be on the edge of the building. lol.
									// See types.h for what a door looks like.
protected:
	// Properties
	bool clicked;

};

#endif
