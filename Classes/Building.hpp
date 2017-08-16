/*
 * Building.h
 * - a building
 *
 */

#ifndef BUILDING_H
#define BUILDING_H

#include <iostream>

#include "types.h"
#include "EventResponder.hpp"
#include "NavMap.hpp"
#include "EventHandler.hpp"


class Building : public EventResponder
{
public:
	Building(NavMap *, int posX, int posY);
	~Building();

	// Methods
	void receiveEvent(Event *ev);	// Handle mouse events
	char col();						// Temporary coloration-signifier

	// Properties
	bool destroyed;

protected:
	// Properties
	NavMap *navmap;
	bool clicked;
	
};

#endif
