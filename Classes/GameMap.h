/*
 * GameMap.h - map data for a level.
 *
 * The GameMap thinks solely in "blocks" - the squares that make up the game map – it does not honour
 * the existence of pixels.
 *
 * Fundamentally, its purpose is to hold the block-coordinates of DrawnObjects in the game. It implements methods
 * for adding and removing those objects, and updating their positions, which must be used when a building is placed,
 * a unit moves between two blocks, and so on.
 *
 */

#ifndef GAMEMAP_H
#define GAMEMAP_H

// #include <iostream>
// #include <vector>
// #include <list>
// 
// using namespace std;


class GameMap
{
public:
	GameMap(int columns, int rows);
	~GameMap();

	// Properties
	int w, h;
	
	// Methods
	//void memmap_addObject(DrawnObject *object);
	//void memmap_removeObject(DrawnObject *object);

protected:
	// Memory mapped list of things at each integer location
	//vector<MapLocation*> maplocs;
	
};

#endif