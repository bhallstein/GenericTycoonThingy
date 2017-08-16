/*
 * Placeable.hpp - sort of an abstract placement manager for MappedObj
 *
 */

#ifndef PLACEABLE_H
#define PLACEABLE_H

#include <iostream>
#include <map>

#include "W.h"

class PlaceableManager;

class Placeable : public W::MappedObj {
public:
	Placeable(PlaceableManager *);
	~Placeable();
	bool activate();
	void deactivate();
	void receiveEvent(W::Event *ev);
	PlaceableManager *mngr;
};

#endif
