/*
 * Level.h - Controls a level, including all the objects therein
 *
 */
#ifndef LEVEL_H
#define LEVEL_H

#include <iostream>
#include <vector>
#include <list>
#include <string>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "types.h"
#include "GameMap.hpp"
#include "EventResponder.hpp"
#include "Placeable.hpp"
#include "Building.hpp"
#include "Unit.hpp"


class Level
{
public:
	Level(std::string fileName, GameMap *_gamemap); //we'll need to pass in which level to load, at a later point
	~Level();

	// Functions
	void draw(sf::RenderWindow& window, int block_width, int block_height);
	Placeable *createPlaceable();
	Unit *createUnit();
	boost::property_tree::ptree readLevel(std::string fileName);
	void buildLevel(boost::property_tree::ptree levelFile);

	// Properties
	int columns, rows;
	std::vector<Building*> buildings;
	std::vector<Placeable*> placeables;
	std::vector<Unit*> units;

protected:
	
	int w, h; 					// Blocks wide/tall.
	GameMap *gamemap;
	int framecount;
};

#endif
