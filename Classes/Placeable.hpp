/*
 * Placeable.h
 *
 */

#ifndef PLACEABLE_H
#define PLACEABLE_H

#include <iostream>

#include "types.h"
#include "EventResponder.hpp"
#include "GameMap.hpp"


class Placeable : public EventResponder
{
public:
	Placeable(GameMap *);
	~Placeable();

	// Methods
	void receiveEvent(Event *ev, EventResponder **p_e_r);	// Override to handle events
	char col();		// Temporary coloration-signifier thingum

	// Properties
	bool destroyed;

protected:

	// Methods
	void setPosition(int _x, int _y);

	// Properties
	GameMap *gamemap;
	enum mode_types { PLACEMENT, PLACED } mode;
	bool clicked;
	
};

#endif
