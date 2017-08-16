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
	Building(NavMap *, EventHandler *, int posX, int posY);
	~Building();

	// Methods
	void receiveEvent(Event *ev);	// Handle events
	char col();						// Temporary coloration-signifier thingum

	// Properties
	bool destroyed;

protected:

	// Properties
	NavMap *navmap;
	EventHandler *eventHandler;
	bool clicked;
	
};

#endif
