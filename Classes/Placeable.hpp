/*
 * Placeable.h
 *
 */

#ifndef PLACEABLE_H
#define PLACEABLE_H

#include <iostream>

#include "types.h"
#include "EventResponder.hpp"
#include "NavMap.hpp"
#include "View.hpp"


class Placeable : public EventResponder
{
public:
	Placeable(NavMap *, View *);
	~Placeable();

	// Methods
	void receiveEvent(Event *ev);
	char col();		// Temporary coloration-signifier thingum

	// Properties
	bool destroyed;

protected:

	// Methods
	void setPosition(int _x, int _y);

	// Properties
	NavMap *navmap;
	View *view;
	enum mode_types { PLACEMENT, PLACED } mode;
	bool clicked;
	
};

#endif
