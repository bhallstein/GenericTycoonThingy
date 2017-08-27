/*
 * Generic Tycoon Thingy
 *
 * ==================
 *  SpawnPoint.hpp
 * ==================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#ifndef SPAWNPOINT_H
#define SPAWNPOINT_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

#include "PlaceableManager.hpp"
#include "Serializable.hpp"

namespace W {
	class NavMap;
}

class SpawnPoint : public PlaceableManager {
public:
	SpawnPoint(LevelState *, LevelMap *, LevelView *, W::NavMap *, bool _placeableMode);
	~SpawnPoint();
	void _setUp();
	void update() { }
	
protected:
	// Placeable overrides
	void placementLoopStarted();
	void placementLoopUpdate();
	void placementLoopCancelled();
	void placementLoopSucceeded();
	bool canPlace(const W::position &);
	
};

#endif
