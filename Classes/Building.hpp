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


class Building : EventResponder
{
public:
	Building(GameMap *);
	~Building();

	// Methods
	void receiveEvent(Event *ev, EventResponder **p_e_r);		// Override receive event method
	char col();		// Temporary coloration-signifier thingum

	// Properties
	int x, y;		// Integer block coordinates
	int w, h;		//

	bool destroyed;

protected:

	// Methods
	void setPosition(int x, int y);

	// Properties
	GameMap *gamemap;
	enum mode_types { PLACEMENT, PLACED } mode;
	bool clicked;
	
};

#endif
