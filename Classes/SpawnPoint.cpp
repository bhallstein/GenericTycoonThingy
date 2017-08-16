#include "SpawnPoint.hpp"

SpawnPoint::SpawnPoint(int _x, int _y, std::string _name, int _rate) :
	x(_x), y(_y), name(_name), rate(_rate), rateCounter(0)
{
	// hai, spawny
}

bool SpawnPoint::spawn(intcoord *c)
{
	bool spawn_ahoy = rate == ++rateCounter;
	if (spawn_ahoy)
		rateCounter = 0, c->x = x, c->y = y;
	return spawn_ahoy;
}
