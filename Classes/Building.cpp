#include "Building.hpp"

Building::Building(NavMap *_navmap, int posX, int posY) {
	x = posX, y = posY;
	
	// Set up event responder block list
	intcoord c;
	for (int j=0; j < 4; j++) for (int i=0; i < 6; i++)
		c.x = i, c.y = j, resp_blocks.push_back(c);
	c.x = 2, c.y = 4, resp_blocks.push_back(c);
	c.x = 3, c.y = 4, resp_blocks.push_back(c);

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
