#include "Placeable.hpp"

Placeable::Placeable(NavMap *_navmap, View *_view) : MappedObj(0, 0), navmap(_navmap), view(_view), clicked(false), destroyed(false), mode(PLACEMENT)
{
	intcoord p[] = {
		{0,0}, {1,0},
		{0,1}, {1,1},	{-1,-1}
	};
	setGroundPlan(p);
}
Placeable::~Placeable()
{
	std::cout << "placeable destruct" << std::endl;
}

void Placeable::receiveEvent(Event *ev) {
	if (mode == PLACEMENT) {
		if (ev->type == MOUSEMOVE) {
			x = ev->x, y = ev->y;
		}
		else if (ev->type == LEFTCLICK) {
			for (int i=0, n = ground_plan.size(); i < n; i++) {
				intcoord c = ground_plan[i];
				if (!navmap->isPassableAt(c.x + x, c.y + y))
					return;							// Check if area is passable
			}
			mode = PLACED;
			view->relinquishPrivilegedEventResponderStatus(this);
			view->addResponder(this);
			navmap->addImpassableObject(this);
		}
		else if (ev->type == RIGHTCLICK) {
			view->relinquishPrivilegedEventResponderStatus(this);
			destroyed = true;
		}
	}
	else if (mode == PLACED) {
		if (ev->type == LEFTCLICK)
			clicked = !clicked;
	}
}

sf::Color Placeable::col() {
	return (mode == PLACEMENT ? sf::Color(255,255,255,255) : clicked ? sf::Color(255,0,0,255) : sf::Color(0,0,0,255));
}
