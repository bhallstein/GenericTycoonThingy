/*
 * GameMap.hpp - map data for a level.
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
#include <queue>

#include "types.h"
#include "Event.hpp"
#include "EventResponder.hpp"
#include "BinaryHeap.hpp"


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
	std::list<MapLoc*> neighbours;		// Pointers to passable neighbour nodes. For pathfinding.
	float min_dist;
	MapLoc *route_prev;
	
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
	void setDimensions(int _w, int _h);	// This should be called immediately after the constructor.
	void addResponder(EventResponder *resp);
	void removeResponder(EventResponder *resp);
	void dispatchEvent(Event *, EventResponder **p_e_r);
	void makeImpassable(int atX, int atY);
	void makePassable(int atX, int atY);
	void addImpassableObject(EventResponder *resp);
	void removeImpassableObject(EventResponder *resp);
	bool isPassableAt(int atX, int atY);
	bool getRoute(int fromX, int fromY, int toX, int toY, std::vector<MapLoc*> *route);

protected:
	// Properties
	std::vector<MapLoc> maplocs;		// A w*h-sized array of MapLocs
	
};

#endif
