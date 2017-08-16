#include "Building.hpp"

Building::Building(GameMap *_gamemap, EventHandler *_eventHandler, int posX, int posY) {
	eventHandler = _eventHandler;
	x = posX, y = posY;
	w = 6, h = 4;
	clicked = false;
	
	gamemap = _gamemap;
	
	// Builing state relevant to the LevelMap.
	destroyed = false;
}
Building::~Building()
{
	//gamemap->removeResponder(this);
	gamemap->removeImpassableObject(this);
}

void Building::receiveEvent(Event *ev) {
	if (ev->type == LEFTCLICK)
		clicked = !clicked;
	else if (ev->key == K_L)
		std::cout << "Building " << this << " received L!" << std::endl;
}

char Building::col() {
	return (clicked ? 'l' : 'b');
}
