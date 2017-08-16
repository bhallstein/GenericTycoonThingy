#include "GameMap.hpp"

/****   MapLoc implementation   ****/

MapLoc::MapLoc() {
	// Constructor
}
MapLoc::~MapLoc() {
	// Destructor
}

void MapLoc::addResponder(EventResponder *x) {
	// Add an eventresponder to the list thereof
	// std::cout << responderList.size() << " -> ";
	responderList.push_back(x);
	// std::cout << responderList.size() << std::endl;
}
void MapLoc::removeResponder(EventResponder *x) {
	// Remove an eventresponder from the list thereof
	// std::cout << responderList.size() << " -> ";
	responderList.remove(x);
	// std::cout << responderList.size() << std::endl;
}
void MapLoc::dispatchEvent(Event *ev, EventResponder **p_e_r) {
	for (std::list<EventResponder*>::iterator i = responderList.begin(); i != responderList.end(); i++)
		(*i)->receiveEvent(ev, p_e_r);
}


/****   GameMap implementation   ****/

GameMap::GameMap() {
	// Constructor
}
GameMap::~GameMap() {
	maplocs.clear();
}

void GameMap::setDimensions(int _w, int _h) {
	w = _w, h = _h;
	maplocs.resize(w * h);
}

void GameMap::addObject(EventResponder *object, int atX, int atY, int obj_w, int obj_h) {
	if (atX < 0 || atY < 0  || atX + obj_w >= w || atY + obj_h >= h)
		return;
	// std::cout << "Adding object to responders at " << atX << "," << atY << "... " << std::endl;
	for (int j = atY; j < atY + obj_h; j++)
		for (int i = atX; i < atX + obj_w; i++) {
			std::cout << i << "," << j << " ";
			maplocs[j*w + i].addResponder(object);
		}
	// std::cout << std::endl << std::endl;
}
void GameMap::removeObject(EventResponder *object, int atX, int atY, int obj_w, int obj_h) {
	if (atX < 0 || atY < 0  || atX >= w - obj_w || atY >= h - obj_h)
		return;
	// std::cout << "Removing object from responders at " << atX << "," << atY << "... " << std::endl;
	for (int j = atY; j < atY + obj_h; j++)
		for (int i = atX; i < atX + obj_w; i++) {
			std::cout << i << "," << j << ": ";
			maplocs[j*w + i].removeResponder(object);
		}
	// std::cout << std::endl << std::endl;
}
void GameMap::dispatchEvent(Event *ev, EventResponder **p_e_r) {
	maplocs[ev->y * w + ev->x].dispatchEvent(ev, p_e_r);
}
