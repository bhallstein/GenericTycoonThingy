/*
 * Generic Tycoon Thingy
 *
 * ========================
 *  PlaceableManager.cpp
 * ========================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#include "PlaceableManager.hpp"

class LevelMap;

PlaceableManager::PlaceableManager(LevelMap *_lm, LevelView *_lv, W::NavMap *_nm, bool _placeableMode) :
	TLO(_lm, _lv, _nm),
	placeableMode(_placeableMode), placeable(this, levelView)
{
	// Hai managey
}

bool PlaceableManager::init(const W::position &_pos) {
	if (placeableMode)
		return pickUp();
	else
		return attemptToPlace(_pos);
}
bool PlaceableManager::init() {
	return init(rct.pos);
}

bool PlaceableManager::pickUp() {
	if ((placeableMode = placeable.activate())) {
		placeable.pos = rct.pos;
		placementLoopStarted();
	}
	else W::log << "Couldn't initialize PlaceableManager: placeable would not activate" << std::endl;
	return placeableMode;
}
bool PlaceableManager::attemptToPlace(const W::position &_pos) {
	if (!canPlace(_pos))
		return false;
	rct.setPos(_pos);
	placeableMode = false;
	placementLoopSucceeded();
	return true;
}
void PlaceableManager::cancel() {
	placeableMode = false;
	placementLoopCancelled();
}
