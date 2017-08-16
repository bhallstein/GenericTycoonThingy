#include "SpawnPoint.hpp"

SpawnPoint::SpawnPoint(LevelState *_ls, LevelMap *_lm, LevelView *_lv, W::NavMap *_nm, bool _placeableMode) :
	PlaceableManager(_ls, _lm, _lv, _nm, _placeableMode)
{
	// hai spawny
}
SpawnPoint::~SpawnPoint()
{
	UIDManager::unregisterTLO(this);
}
void SpawnPoint::setUp() {
	if (!deserialized)
		uid = UIDManager::getNewUID();
	UIDManager::registerTLO(this);
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
