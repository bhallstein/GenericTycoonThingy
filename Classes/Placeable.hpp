/*
 * Placeable.h
 *
 */

#ifndef PLACEABLE_H
#define PLACEABLE_H

#include <iostream>

#include "types.h"
#include "MappedObj.hpp"

class NavMap;
class ResponderMap;

class Placeable : public MappedObj {
public:
	Placeable(NavMap *, ResponderMap *);
	~Placeable();

	// Methods
	void receiveEvent(Event *ev);
	void update() { }
	colour col();

	// Properties
	bool destroyed;

protected:
	// Properties
	enum mode_types { PLACEMENT, PLACED } mode;
	bool clicked;
	NavMap *navmap;
	ResponderMap *levelResponderMap;
	
};

#endif
