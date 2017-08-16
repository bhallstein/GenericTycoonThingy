/*
 * Building.h
 * - a building
 *
 */

#ifndef BUILDING_H
#define BUILDING_H

#include <iostream>

#include "types.h"
#include "EventResponder.h"


class Building : EventResponder
{
public:
	Building();
	~Building();

	// Methods
	void handleMouseMove(int x, int y);
 	void setPositionFromMouse(int x, int y);
	void getBounds(int *x1, int *y1, int *x2, int *y2);

	void receiveEvent(Event *ev, EventResponder **p_e_r);		// Override receive event method
	char col();		// Temporary coloration-signifier thingum

	// Properties
	int x, y;		// Both in blocks
	int w, h;		//

	bool destroyed;
	enum mode_types { PLACEMENT, PLACED } mode;

protected:
	
};

#endif
