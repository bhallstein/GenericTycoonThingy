#include "Placeable.hpp"

Placeable::Placeable(GameMap *_gamemap, View *_view) : gamemap(_gamemap), view(_view)
{
	mode = PLACEMENT;
	x = y = -1000;
	w = 6; h = 4;
	clicked = false;
	
	// Placeable state relevant to the LevelMap.
	destroyed = false;
}
Placeable::~Placeable()
{
	gamemap->removeImpassableObject(this);
}

void Placeable::receiveEvent(Event *ev) {
	if (mode == PLACEMENT) {
		if (ev->type == MOUSEMOVE) {
			setPosition(ev->x, ev->y);
		}
		else if (ev->type == LEFTCLICK) {
			for (int j=y; j < y + h; j++)
				for (int i=x; i < x + w; i++)
					if (!gamemap->isPassableAt(i, j))
						return;							// Check if area is passable
			mode = PLACED;
			view->relinquishPrivilegedEventResponderStatus(this);
			view->addResponder(this);
			gamemap->addImpassableObject(this);
		}
		else if (ev->type == RIGHTCLICK) {
			destroyed = true;
		}
	}
	else if (mode == PLACED) {
		if (ev->type == MOUSEMOVE) {
			//mouseover = true;	// ...but how to unset?
			// Think a better event would be "mouse is over" – simply avails the object of the fact,
			// so it (or the drawing class) can respond accordingly (a highlighted drawing state, for instance.).
		}
		else if (ev->type == LEFTCLICK) {
			clicked = !clicked;
		}
	}
}

void Placeable::setPosition(int _x, int _y) {
	x = _x, y = _y;
}


char Placeable::col() {
	return (mode == PLACEMENT ? 'w' : clicked ? 'r' : 'b');
}
