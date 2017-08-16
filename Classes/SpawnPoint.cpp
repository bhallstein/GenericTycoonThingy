#include "SpawnPoint.hpp"

SpawnPoint::SpawnPoint(LuaHelper *mrLua)
{
	//what's in the box?
	name = mrLua->getfield<std::string>("name");
	x = mrLua->getfield<int>("x");
	y = mrLua->getfield<int>("y");
	rate = mrLua->getfield<int>("rate");

	rateCounter = 0;
}

std::vector<int> SpawnPoint::Spawn()
{
	std::vector<int> thereturnofthespawn;

	if(rate == rateCounter++)
	{
		thereturnofthespawn.push_back(x);
		thereturnofthespawn.push_back(y);

		rateCounter = 0; //reset :)
	}

	return thereturnofthespawn;
}