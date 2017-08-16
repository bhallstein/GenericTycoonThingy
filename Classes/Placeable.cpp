#include "Placeable.hpp"

Placeable::Placeable(GameMap *map)
{
	mode = PLACEMENT;
	x = y = -1000;
	w = 6; h = 4;
	clicked = false;
	
	gamemap = map;
	
	// Placeable state relevant to the LevelMap.
	destroyed = false;
}
Placeable::~Placeable()
{
	// Remove from memory map
	gamemap->removeResponder(this);
	gamemap->removeImpassableObject(this);
}

void Placeable::receiveEvent(Event *ev, EventResponder **p_e_r) {
	if (mode == PLACEMENT) {
		if (ev->type == MOUSEMOVE) {
			this->setPosition(ev->x, ev->y);
		}
		else if (ev->type == LEFTCLICK) {
			for (int j=y; j < y + h; j++)
				for (int i=x; i < x + w; i++)
					if (!gamemap->isPassableAt(i, j))
						return;							// Check if area is passable
			mode = PLACED;
			gamemap->addImpassableObject(this);
			*p_e_r = NULL;
		}
		else if (ev->type == RIGHTCLICK) {
			destroyed = true;
			*p_e_r = NULL;
		}
	}
	else if (mode == PLACED) {
		if (ev->type == MOUSEMOVE) {
			//mouseover = true;	// ...but how to unset?
			// Think a better event would be "mouse is over" – avails the object of the fact once per frame,
			// so it (or the drawing class) can respond accordingly (highlighted drawing, for instance.)
		}
		else if (ev->type == LEFTCLICK) {
			clicked = !clicked;
		}
	}
	// std::cout << "Placeable pos: " << x << "," << y << std::endl;
}

void Placeable::setPosition(int _x, int _y) {
	if (x == _x && y == _y) return;
	gamemap->removeResponder(this);
	x = _x, y = _y;
	gamemap->addResponder(this);
}


char Placeable::col() {
	return (mode == PLACEMENT ? 'w' : clicked ? 'l' : 'b');
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
