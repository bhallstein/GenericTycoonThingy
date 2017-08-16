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
#include <list>
#include <vector>
// #include <list>

#include "EventResponder.hpp"


class MapLoc
{
public:
	MapLoc();
	~MapLoc();
	
	// Methods
	void addObject(EventResponder *x);
	void removeObject(EventResponder *x);
	
protected:
	std::list<EventResponder*> responderList;
	
};


class GameMap
{
public:
	GameMap();
	~GameMap();

	// Properties
	int w, h;
	
	// Methods
	void setDimensions(int _w, int _h);
	void addObject(EventResponder *object, int atX, int atY, int obj_w = 1, int obj_h = 1);
	void removeObject(EventResponder *object, int atX, int atY, int obj_w = 1, int obj_h = 1);

protected:
	// Properties
	std::vector<MapLoc> maplocs;	// A [w*h]-sized array of MapLocs
	
};

#endif
