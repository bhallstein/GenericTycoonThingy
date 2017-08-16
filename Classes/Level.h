/*
 * Level.h - Controls a level, including all the objects therein
 *
 */
#ifndef LEVEL_H
#define LEVEL_H

#include <iostream>
#include <vector>
#include <list>
#include "Building.h"

using namespace std;

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


class Level
{
public:
	Level(int _rows, int _columns, int _width, int _height);
	~Level();

	// Functions
	void draw(sf::RenderWindow& window);
	Building *createBuilding();

	// Properties
	int columns, rows;
	vector<Building> buildings;

protected:
	// Pixel size of a block on the map
	sf::Vector2i block_size;
};

#endif