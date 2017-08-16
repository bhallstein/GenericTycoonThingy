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
#include <vector>

#include "types.hpp"
#include "MisterHeapy.hpp"

class NavNode
{
public:
	NavNode();
	~NavNode();
	
	// Methods
	void addNeighbour(NavNode *neighbour);
	void removeNeighbour(NavNode *neighbour);
	bool hasNeighbour(NavNode *n);
	
	bool operator< (NavNode *);		// For ordering in MisterHeapy
	void setComparand(float);		// For updating by MisterHeapy
	
	// Properties
	int x, y;
	bool passable;
	std::vector<NavNode *> neighbours;		// Pointers to passable neighbour nodes. For pathfinding.
	float min_dist;

	NavNode *route_prev;
	
protected:
	
};


class MappedObj;
class Building;

class NavMap
{
public:
	NavMap(int w, int h);
	~NavMap();

	// Properties
	int w, h;
	
	// Methods
	void addImpassableObject(MappedObj *);
	void removeImpassableObject(MappedObj *);
	void addBuilding(Building *);
	void removeBuilding(Building *);
	bool isPassableAt(int atX, int atY);
	NavNode* nodeAt(int atX, int atY);
	
	bool getRoute(int fromX, int fromY, int toX, int toY, std::vector<NavNode*> *route);

protected:
	// Properties
	std::vector<NavNode> nodes;		// A w*h-sized array of NavNodes
	MisterHeapy<NavNode*, float> open_nodes;
	
	// Methods
	void makeImpassable(int atX, int atY);
	void makePassable(int atX, int atY);
};

#endif
