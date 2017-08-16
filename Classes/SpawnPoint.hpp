/*
 * SpawnPoint.hpp - a spawn point
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

class W::NavMap;

class SpawnPoint : public PlaceableManager {
public:
	SpawnPoint(LevelState *, LevelMap *, LevelView *, W::NavMap *, bool _placeableMode);
	
	void update() { }
	void setUp() { }
	
protected:
	// Placeable overrides
	void placementLoopStarted();
	void placementLoopUpdate();
	void placementLoopCancelled();
	void placementLoopSucceeded();
	bool canPlace(const W::position &);
	
};

#endif
