#include "MappedObj.hpp"
#include "Placeable.hpp"

MappedObj::MappedObj(ResponderMap *_rm, bool _placeableMode) :
	rm(_rm), placeableMode(_placeableMode), placeable(NULL), destroyed(false), hover(false), a(0), b(0)
{
	x = y = rotation = 0;
}
MappedObj::~MappedObj()
{
	rm->removeMappedObj(this);
	rm->unsubscribeFromAllKeys(this);
	if (placeable) delete placeable;
}

bool MappedObj::init(int _x, int _y) {
	if (placeableMode)
		try {
			placeable = new Placeable(this, rm);
		} catch (MsgException &exc) {
			std::string s = "Could not initialise MappedObj: ";
			s += exc.msg;
			W::log(s.c_str());
			return false;
		}
	else
		return attemptToPlace(_x, _y);
	return true;
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
