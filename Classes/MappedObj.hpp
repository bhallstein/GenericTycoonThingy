/*
 * MappedObj.hpp - abstract base class for objects to be drawn on the map
 *
 */

#ifndef MAPPEDOBJ_H
#define MAPPEDOBJ_H

#include <vector>

#include "types.hpp"
#include "EventResponder.hpp"
#include "ResponderMap.hpp"
#include "../W.hpp"

class Placeable;

class MappedObj : public EventResponder {
public:
	MappedObj(ResponderMap *, bool _placeableMode);
	~MappedObj();
	bool init(int _x, int _y);
	
	bool pickUp();
	bool attemptToPlace(int _x, int _y);
	virtual void finalizePlacement() { };	// Override, for instance to add to a navmap
	virtual bool canPlace(int _x, int _y) = 0;
	virtual void update() = 0;
	virtual const char * col() = 0;
	bool overlapsWith(int _x, int _y);
	
	void setGroundPlan(std::vector<intcoord> *);
	
	float a, b;
	std::vector<intcoord> groundplan;
	bool destroyed;
	bool hover;
	float rotation;
	
	ResponderMap *rm;
	bool placeableMode;
	Placeable *placeable;
};

#endif
