/*
 * Building.h - a building
 *
 */

#ifndef BUILDING_H
#define BUILDING_H

#include <iostream>

#include "types.h"
#include "MappedObj.hpp"


class Building : public MappedObj
{
public:
	Building(int _x, int _y);
	~Building();

	// Methods
	void receiveEvent(Event *ev);	// Handle mouse events
	char col();						// Temporary coloration-signifier

	// Properties
	bool destroyed;

protected:
	// Properties
	bool clicked;

};

#endif
