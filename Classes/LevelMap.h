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

	// Functions
	void draw(sf::RenderWindow& window);
	Building *createBuilding();

	//void memmap_addObject(DrawnObject *object);
	//void memmap_removeObject(DrawnObject *object);

	// Properties
	int columns, rows;
	vector<Building> buildings;

protected:
	// Memory mapped list of things at each integer location
	vector<MapLocation*> maplocs;

	// Pixel size of a block on the map
	sf::Vector2i block_size;
};

#endif