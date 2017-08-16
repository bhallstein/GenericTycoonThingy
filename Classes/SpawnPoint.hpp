/*
	spawnPoint.hpp - a spawn point.
*/

#ifndef SPAWNPOINT_H
#define SPAWNPOINT_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

#include "Unit.hpp"

class SpawnPoint
{
public:
	SpawnPoint(int _x, int _y, std::string _name, int rate);
	~SpawnPoint();

	bool spawn(intcoord *);
	void updateRate(int newRate);
	void addCustomer(std::string customer, int rate);
	void removeCustomer(std::string customer);

	std::map<std::string, int[2]> customers;//to store both the types and the rate at which they will spawn. 2nd element in array is the counter.
	
	//properties
	bool isSpawning;
	int x, y;
	std::string name;
	int rate; //global rate right now; no unit typing
	int rateCounter; //temp!
	
protected:
	
};

#endif
