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

class Placeable {
public:
	Placeable(PlaceableManager *);
	~Placeable();
	bool activate();
	void deactivate();
	W::EventPropagation::T mouseEvent(W::Event *);
	PlaceableManager *mngr;
	W::position pos;
};

#endif
