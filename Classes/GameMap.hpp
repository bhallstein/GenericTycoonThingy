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

#include <iostream>
#include <list>
#include <vector>
// #include <list>

#include "Event.hpp"
#include "EventResponder.hpp"


class MapLoc
{
public:
	MapLoc();
	~MapLoc();
	
	// Methods
	void dispatchEvent(Event *ev, EventResponder **p_e_r);
	void addResponder(EventResponder *x);
	void removeResponder(EventResponder *x);
	void addNeighbour(MapLoc *neighbour);
	void removeNeighbour(MapLoc *neighbour);
	
	// Properties
	int x, y;
	bool passable;
	
protected:
	std::list<EventResponder*> responderList;
	std::list<MapLoc*> neighbours;		// Pointers to passable neighbour nodes. Used for pathfinding.
	
};


class GameMap
{
public:
	GameMap();
	~GameMap();

	// Properties
	int w, h;
	
	// Methods
	void setDimensions(int _w, int _h);	// This should be called immediately after the constructor.
	void addResponder(EventResponder *resp);
	void removeResponder(EventResponder *resp);
	void dispatchEvent(Event *, EventResponder **p_e_r);
	void makeImpassable(int atX, int atY);
	void makePassable(int atX, int atY);
	void addImpassableObject(EventResponder *resp);
	void removeImpassableObject(EventResponder *resp);
	bool isPassableAt(int atX, int atY);

protected:
	// Properties
	std::vector<MapLoc> maplocs;	// A [w*h]-sized array of MapLocs
	
};

#endif
