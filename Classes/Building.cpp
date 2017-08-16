#include "Building.hpp"

Building::Building(GameMap *map)
{
	x = y = -1000;
	w = 6; h = 4;
	clicked = false;
	
	gamemap = map;
	
	// Builing state relevant to the LevelMap.
	destroyed = false;
}
Building::~Building()
{
	// Remove from memory map
	gamemap->removeResponder(this);
	gamemap->removeImpassableObject(this);
}

void Building::receiveEvent(Event *ev, EventResponder **p_e_r) {
	if (ev->type == MOUSEMOVE) {
		//mouseover = true;	// ...but how to unset?
	}
	else if (ev->type == LEFTCLICK) {
		clicked = !clicked;
	}
}

void Building::setPosition(int _x, int _y) {
	if (x == _x && y == _y) return;
	gamemap->removeResponder(this);
	x = _x, y = _y;
	gamemap->addResponder(this);
}


char Building::col() {
	return (clicked ? 'l' : 'b');
}
