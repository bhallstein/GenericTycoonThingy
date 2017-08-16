#include "MappedObj.hpp"
#include "Placeable.hpp"

MappedObj::MappedObj(ResponderMap *_rm, bool _placeableMode) :
	rm(_rm), placeableMode(_placeableMode), placeable(NULL), destroyed(false), hover(false), a(0), b(0)
{
	x = y = rotation = 0;
	placeable = new Placeable(this, rm);
}
MappedObj::~MappedObj()
{
	rm->removeMappedObj(this);
	rm->unsubscribeFromAllKeys(this);
	if (placeable) delete placeable;
}

bool MappedObj::init(int _x, int _y) {
	if (placeableMode) {
		if (!placeable->activate()) {
			W::log("Couldn't initialize MappedObj: placeable would not activate");
			return false;
		}
		return true;
	}
	else
		return attemptToPlace(_x, _y);
}

bool MappedObj::pickUp() {
	placeable->x = x, placeable->y = y;
	a = b = 0;
	return placeableMode = placeable->activate();
}
bool MappedObj::attemptToPlace(int _x, int _y) {
	if (!canPlace(_x, _y)) return false;
	x = _x, y = _y;
	rm->addMappedObj(this);
	finalizePlacement();
	placeableMode = false;
	return true;
}

void MappedObj::setGroundPlan(std::vector<intcoord> *_p) {
	groundplan = *_p;
}

bool MappedObj::overlapsWith(int _x, int _y) {
	for (int i=0, n = groundplan.size(); i < n; i++) {
		intcoord c = groundplan[i];
		if (x + c.x == _x && y + c.y == _y)
			return true;
	}
	return false;
}
