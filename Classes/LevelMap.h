/*
 * LevelMap.h - Defines the map data, including buildings and units, for a level
 *
 */
#ifndef LEVELMAP_H
#define LEVELMAP_H

#include <iostream>
#include <vector>
#include "Building.h"

using namespace std;

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class LevelMap
{
public:
	LevelMap(int w, int h);
	~LevelMap();

	// Properties
	int columns, rows;
	vector<Building> buildings;

	// Getters
	int numberOfBuildings();

	// Functions
	void draw(sf::RenderWindow& window);
	Building *createBuilding();
	void destroyBuilding(); //currently does last one in the vector; later should take a building reference

private:

};

#endif