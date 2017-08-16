/*
 * Level.h - Controls a level, including all the objects therein
 *
 */
#ifndef LEVEL_H
#define LEVEL_H

#include <iostream>
#include <vector>
#include <list>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "types.h"
#include "Building.h"


class Level
{
public:
	Level(int _w, int _h);
	~Level();

	// Functions
	void draw(sf::RenderWindow& window, int block_width, int block_height);
	Building *createBuilding();

	// Properties
	int columns, rows;
	std::vector<Building> buildings;

protected:
	int w, h; 					// Blocks wide/tall.
};

#endif
