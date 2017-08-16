/*
 * LevelMap.h - Defines the map data, including buildings and units, for a level
 *
 */
#ifndef LEVELMAP_H
#define LEVELMAP_H

#include <vector>
#include "Building.h" 

using namespace std;


class LevelMap
{
public:
	LevelMap(int w, int h);
	~LevelMap();

	// Properties
	int Columns, Rows;
	vector<Building> Buildings;	// Can this be replaced with an array of pointers?

	// Getters
	int numberOfBuildings();

	// Functions
	void Draw(sf::RenderWindow& window);

private:

};

#endif