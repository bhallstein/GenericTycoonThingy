#include "Placeable.hpp"
#include "NavMap.hpp"
#include "ResponderMap.hpp"

Placeable::Placeable(NavMap *_navmap, ResponderMap *_levelRM) :
	MappedObj(-100, -100), navmap(_navmap), levelResponderMap(_levelRM), clicked(false), destroyed(false), mode(PLACEMENT)
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
		if (ev->type == Event::MOUSEMOVE) {
			x = ev->x, y = ev->y;
		}
		else if (ev->type == Event::LEFTCLICK) {
			for (int i=0, n = ground_plan.size(); i < n; i++) {
				intcoord c = ground_plan[i];
				if (!navmap->isPassableAt(c.x + x, c.y + y))
					return;							// Check if area is passable
			}
			mode = PLACED;
			levelResponderMap->relinquishPrivilegedEventResponderStatus(this);
			levelResponderMap->addMappedObj(this);
			navmap->addImpassableObject(this);
		}
		else if (ev->type == Event::RIGHTCLICK) {
			levelResponderMap->relinquishPrivilegedEventResponderStatus(this);
			destroyed = true;
		}
	}
	else if (mode == PLACED) {
		if (ev->type == Event::LEFTCLICK)
			clicked = !clicked;
	}
}

colour Placeable::col() {
	return (mode == PLACEMENT ? _WHITE_ : clicked ? _RED_ : _BLUE_);
}
