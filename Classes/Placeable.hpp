/*
 * Placeable.h
 *
 */

#ifndef PLACEABLE_H
#define PLACEABLE_H

#include <iostream>

#include "types.hpp"
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
	const char * col();

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
