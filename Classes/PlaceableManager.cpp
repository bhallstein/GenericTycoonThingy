#include "PlaceableManager.hpp"

class LevelMap;

PlaceableManager::PlaceableManager(LevelMap *_lm, View__Game *_lv, W::NavMap *_nm, bool _placeableMode) :
  TLO(_lm, _lv, _nm),
  placeableMode(_placeableMode), placeable(this, view__game)
{ }

bool PlaceableManager::init(W::v2f _pos) {
  if (placeableMode)
    return pickUp();
  else
    return attemptToPlace(_pos);
}
bool PlaceableManager::init() {
  return init(rct.position);
}

bool PlaceableManager::pickUp() {
  if ((placeableMode = placeable.activate())) {
    placeable.pos = rct.position;
    placementLoopStarted();
  }
  else W::log << "Couldn't initialize PlaceableManager: placeable would not activate" << std::endl;
  return placeableMode;
}
bool PlaceableManager::attemptToPlace(W::v2f _pos) {
  if (!canPlace(_pos)) {
    return false;
  }
  rct.position = _pos;
  placeableMode = false;
  placementLoopSucceeded();
  return true;
}
void PlaceableManager::cancel() {
  placeableMode = false;
  placementLoopCancelled();
}
