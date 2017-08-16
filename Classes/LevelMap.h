/*
 * LevelMap.h - Defines the map data, including buildings and units, for a level
 *
 */
#ifndef LEVELMAP_H
#define LEVELMAP_H

#include <iostream>
#include <vector>
#include <list>
#include "Building.h"

using namespace std;

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class MapLocation {
public:
	MapLocation();
	~MapLocation();

	void addBuilding(Building* b);
	void removeBuilding(Building* b);
private:
	// "Compared to other vectors and deques, lists perform better in inserting, extracting and moving elements..."
	//List<Unit*> units;
	list<Building*> buildings;
};

class LevelMap
{
public:
	LevelMap(int _rows, int _columns, int _width, int _height);
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

	void moveBuilding(Building *b, int x1, int y1, int x2, int y2);	// If setting initial location, use a negative value for x1 or y1
private:
	// Memory mapped list of things at each integer location
	vector<MapLocation*> maplocs;

	// Pixel size of a block on the map
	sf::Vector2i block_size;
};

#endif