#include "GameMap.hpp"

/*********************************
 *     MapLoc implementation     *
 *********************************/

MapLoc::MapLoc() {
	// Constructor
}
MapLoc::~MapLoc() {
	// Destructor
}

void MapLoc::addObject(EventResponder *x) {
	// Add an eventresponder to the list thereof
	responderList.push_back(x);
}
void MapLoc::removeObject(EventResponder *x) {
	// Remove an eventresponder from the list thereof
	responderList.remove(x);
}


/*********************************
 *    GameMap implementation    *
 *********************************/

GameMap::GameMap()
{
	// Constructor
}
GameMap::~GameMap()
{
	maplocs.clear();
}

void GameMap::setDimensions(int _w, int _h) {
	w = _w, h = _h;
	maplocs.resize(w * h);
}

void GameMap::addObject(EventResponder *object, int atX, int atY, int obj_w, int obj_h) {
	if (atX < 0 || atY < 0  || atX >= w - obj_w || atY >= h - obj_h)
		return;
	maplocs[atY * w + atX].addObject(object);
}
void GameMap::removeObject(EventResponder *object, int atX, int atY, int obj_w, int obj_h) {
	if (atX < 0 || atY < 0  || atX >= w - obj_w || atY >= h - obj_h)
		return;
	maplocs[atY * w + atX].removeObject(object);
}
