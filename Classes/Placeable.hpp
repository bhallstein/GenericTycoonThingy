/*
 * Generic Tycoon Thingy
 *
 * =================
 *  Placeable.hpp
 * =================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

/*
 * Placeable.hpp - sort of an abstract placement manager for TLOs
 *
 */

#ifndef PLACEABLE_H
#define PLACEABLE_H

#include <iostream>
#include <map>

#include "W.h"

class PlaceableManager;
class LevelView;

class Placeable {
public:
	Placeable(PlaceableManager *, LevelView *);
	~Placeable();
	bool activate();
	void deactivate();
	W::EventPropagation::T mouseEvent(W::Event *);
	PlaceableManager *mngr;
	W::position pos;
	LevelView *levelView;
};

#endif
