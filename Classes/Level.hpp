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
#include "GameMap.hpp"
#include "EventResponder.hpp"
#include "Building.hpp"
#include "Unit.hpp"


class Level
{
public:
	Level(int _w, int _h, GameMap *_gamemap);
	~Level();

	// Functions
	void draw(sf::RenderWindow& window, int block_width, int block_height);
	Building *createBuilding();
	Unit *createUnit();

	// Properties
	int columns, rows;
	std::vector<Building*> buildings;
	std::vector<Unit*> units;

protected:
	int w, h; 					// Blocks wide/tall.
	GameMap *gamemap;
	int framecount;
};

#endif
