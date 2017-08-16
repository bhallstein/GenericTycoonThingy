#include "GameMap.h"

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
	things.push_back(x);
}
void MapLoc::removeObject(EventResponder *x) {
	// Remove an eventresponder from the list thereof
	things.remove(x);
}



/*********************************
 *    GameMap implementation    *
 *********************************/

GameMap::GameMap(int columns, int rows)
{
	w = columns, h = rows;
	maplocs.resize(w * h);
}
GameMap::~GameMap()
{
	// Destructor
	maplocs.clear();
}

void GameMap::addObject(EventResponder *object, int atX, int atY) {
	if (atX < 0 || atX >= w || atY < 0 || atY >= h)
		return;
	maplocs[atY * w + atX].addObject(object);
}
void GameMap::removeObject(EventResponder *object, int atX, int atY) {
	if (atX < 0 || atX >= w || atY < 0 || atY >= h)
		return;
	maplocs[atY * w + atX].removeObject(object);
}
