#include "MappedObj.hpp"

MappedObj::MappedObj(ResponderMap *_rm, bool _placeableMode) :
	rm(_rm), placeableMode(_placeableMode), placeable(NULL), destroyed(false), hover(false), a(0), b(0)
{
	x = y = 0;
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
	if (placeable) { delete placeable; placeable = NULL; }
	return true;
}

void MappedObj::setGroundPlan(std::vector<intcoord> *_p) {
	groundplan = *_p;
}
