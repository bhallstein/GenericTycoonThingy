#include "Building.hpp"

Building::Building(int _x, int _y) :
	MappedObj(_x, _y), clicked(false), destroyed(false), time_hovered(0)
{
	intcoord p[] = {
		{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},
		{0,1}, {1,1}, {2,1}, {3,1}, {4,1}, {5,1},
		{0,2}, {1,2}, {2,2}, {3,2}, {4,2}, {5,2},
		{0,3}, {1,3}, {2,3}, {3,3}, {4,3}, {5,3},
		              {2,4}, {3,4},					{-1,-1}
	};
	setGroundPlan(p);
	door d1 = {2,4, DOWNWARD};
	door d2 = {3,4, DOWNWARD};
	doors.push_back(d1);
	doors.push_back(d2);
}
Building::~Building()
{
	std::cout << "building destruct" << std::endl;
}

void Building::receiveEvent(Event *ev) {
	if (ev->type == Event::LEFTCLICK)
		clicked = !clicked;
	else if (ev->key == Event::K_L)
		std::cout << "Building " << this << " detected the L key!" << std::endl;
	else if (ev->type == Event::MOUSEMOVE)
		if (++time_hovered == 60) {			// Eventually want an actual time, not a frame count
			
		}
}

colour_name Building::col() {
	colour_name defaultCol;
	switch(type)
	{
	case HOME:
		defaultCol = BLACK;
		break;
	case BARBER:
		defaultCol = RED;
		break;
	case PIESHOP:
		defaultCol = YELLOW;
		break;
	default:
		defaultCol = WHITE;
	}
	hover = false;
	return (clicked ? BLUE : defaultCol);
}
