#include "Building.hpp"

Building::Building(int _x, int _y) : MappedObj(_x, _y), clicked(false), destroyed(false)
{
	intcoord p[] = {
		{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},
		{0,1}, {1,1}, {2,1}, {3,1}, {4,1}, {5,1},
		{0,2}, {1,2}, {2,2}, {3,2}, {4,2}, {5,2},
		{0,3}, {1,3}, {2,3}, {3,3}, {4,3}, {5,3},
		              {2,4}, {3,4},					{-1,-1}
	};
	setGroundPlan(p);
	door d[] = {
		{2,4, DOWNWARD},
		{3,4, DOWNWARD}
	};
	doors.push_back(d[0]);
	doors.push_back(d[1]);
}
Building::~Building()
{
	std::cout << "building destruct" << std::endl;
}

void Building::receiveEvent(Event *ev) {
	if (ev->type == LEFTCLICK)
		clicked = !clicked;
	else if (ev->key == K_L)
		std::cout << "Building " << this << " received L!" << std::endl;
}

char Building::col() {
	char defaultCol;
	switch(type)
	{
	case HOME:
		defaultCol = 'w';
		break;
	case BARBER:
		defaultCol = 'r';
		break;
	case PIESHOP:
		defaultCol = 'y';
		break;
	default:
		defaultCol = 'b';
	}
	return (clicked ? 'l' : defaultCol);
}
