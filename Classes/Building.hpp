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
#include "GameMap.hpp"


class Building : public EventResponder
{
public:
	Building(GameMap *);
	~Building();

	// Methods
	void receiveEvent(Event *ev, EventResponder **p_e_r);	// Override to handle events
	char col();		// Temporary coloration-signifier thingum
	
	void setPosition(int x, int y);

	// Properties
	bool destroyed;

protected:

	// Properties
	GameMap *gamemap;
	bool clicked;
	
};

#endif
