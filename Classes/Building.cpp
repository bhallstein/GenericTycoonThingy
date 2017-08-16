#include "Building.hpp"

Building::Building(NavMap *_navmap, int posX, int posY) {
	x = posX, y = posY;
	w = 6, h = 4;
	clicked = false;
	
	navmap = _navmap;
	navmap->addImpassableObject(this);
	
	// Builing state relevant to the LevelMap.
	destroyed = false;
}
Building::~Building()
{
	std::cout << "building destruct" << std::endl;
	navmap->removeImpassableObject(this);
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
