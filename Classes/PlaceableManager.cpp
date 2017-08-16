#include "PlaceableManager.hpp"
#include "Placeable.hpp"

PlaceableManager::PlaceableManager(bool _placeableMode) :
	placeableMode(_placeableMode), placeable(NULL)
{
	pos.x = pos.y = 0;
	rotation = 0;
	placeable = new Placeable(this);
}
PlaceableManager::~PlaceableManager()
{
	if (placeable) delete placeable;
}

bool PlaceableManager::init(int _x, int _y) {
	if (placeableMode) {
		if (!placeable->activate()) {
			W::log << "Couldn't initialize PlaceableManager: placeable would not activate" << std::endl;
			return false;
		}
		return true;
	}
	else
		return attemptToPlace(_x, _y);
}

bool PlaceableManager::pickUp() {
	placeable->pos = pos;
	return placeableMode = placeable->activate();
}
bool PlaceableManager::attemptToPlace(int _x, int _y) {
	if (!canPlace(_x, _y)) return false;
	pos.x = _x, pos.y = _y;
	finalizePlacement();
	placeableMode = false;
	return true;
}

//void PlaceableManager::setGroundPlan(std::vector<intcoord> *_p) {
//	groundplan = *_p;
//}
