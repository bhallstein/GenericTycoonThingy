/*
 * Generic Tycoon Thingy
 *
 * ==================
 *  SpawnPoint.cpp
 * ==================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#include "SpawnPoint.hpp"

SpawnPoint::SpawnPoint(LevelState *_ls, LevelMap *_lm, LevelView *_lv, W::NavMap *_nm, bool _placeableMode) :
	PlaceableManager(_ls, _lm, _lv, _nm, _placeableMode)
{
	// hai spawny
}
SpawnPoint::~SpawnPoint()
{
	// bai spawny
}
void SpawnPoint::_setUp() {
	
}
void SpawnPoint::placementLoopStarted() {
	// Update drawn representation properties
}
void SpawnPoint::placementLoopUpdate() {
	// Update drawn repr position
}
void SpawnPoint::placementLoopCancelled() {
	// Reset drawn representation properties
}
void SpawnPoint::placementLoopSucceeded() {
	// Reset drawn repr properties
}
bool SpawnPoint::canPlace(const W::position &pos) {
	return true;
}
