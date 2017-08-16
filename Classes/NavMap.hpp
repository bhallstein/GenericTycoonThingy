/*
 * NavMap.hpp - map data for a level.
 *
 * The NavMap thinks solely in "blocks" - the squares that make up the game map – it does not honour
 * the existence of pixels.
 *
 * Fundamentally, its purpose is to hold the block-coordinates of objects in the game. It implements methods
 * for adding and removing those objects, and updating their positions, which must be used when a building is placed,
 * a unit moves between two blocks, and so on.
 *
 */

#ifndef NavMap_H
#define NavMap_H

#include <iostream>
#include <list>
#include <vector>

#include "types.h"
#include "Event.hpp"
#include "EventResponder.hpp"
#include "MisterHeapy.hpp"

using std::cout;
using std::endl;


class NavNode
{
public:
	NavNode();
	~NavNode();
	
	// Methods
	void addNeighbour(NavNode *neighbour);
	void removeNeighbour(NavNode *neighbour);
	
	bool operator< (NavNode*);		// For ordering in MisterHeapy
	void setComparator(float);      // For updating by MisterHeapy
	
	// Properties
	int x, y;
	bool passable;
	std::list<NavNode*> neighbours;		// Pointers to passable neighbour nodes. For pathfinding.
	float min_dist;

	NavNode *route_prev;
	
protected:
	
};


class NavMap
{
public:
	NavMap(int w, int h);
	~NavMap();

	// Properties
	int w, h;
	
	// Methods
	void addImpassableObject(EventResponder *resp);
	void removeImpassableObject(EventResponder *resp);
	bool isPassableAt(int atX, int atY);
	
	bool getRoute(int fromX, int fromY, int toX, int toY, std::vector<NavNode*> *route);

protected:
	// Properties
	std::vector<NavNode> maplocs;		// A w*h-sized array of NavNodes
	
	// Methods
	void makeImpassable(int atX, int atY);
	void makePassable(int atX, int atY);
};

#endif