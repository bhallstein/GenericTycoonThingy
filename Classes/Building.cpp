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


#ifdef what_is_a_building

  What is a building?
  
  A building is an area displayed at a paticular location on the map.
  When it is created, the user has the chance to specify where it can be built.
  	- some parts of the map may be buildable, others not
  
  A building also defines behaviours of some other objects.
  A staff unit may seek out or be assigned to a building
  
  A building generates its own internal mapping region, also – i.e. they define a free movement zone,
  and form a node in the pathfinding system’s nodelist.
  
  Buildings are like other things in the game (units, obstacles, implements, furniture...) in
  some key respects – for example, each has a sprite, and managed the display of that sprite
  (or returns it for display by some more powerful parent class.)

#endif
