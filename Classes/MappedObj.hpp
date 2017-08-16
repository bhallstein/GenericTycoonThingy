/*
 * MappedObj.hpp - base class for objects to be drawn on the map
 *
 */

#ifndef MAPPEDOBJ_H
#define MAPPEDOBJ_H

#include <vector>

#include "types.hpp"
#include "EventResponder.hpp"
#include "LuaHelper.hpp"

class MappedObj : public EventResponder
{
public:
	MappedObj(int _x, int _y) : a(0), b(0) { x = _x, y = _y; }
	~MappedObj() { }
	
	// Methods
	virtual void update() = 0;			// TLOs will generally want to handle updates
	virtual const char * col() = 0;		// Temporary: override to return colour the object should be drawn
	void setGroundPlan(std::vector<intcoord> *p) {
		groundplan = *p;
	}
	
	// Properties
	float a, b;							// Floating pt offset from integer location (default 0)
	std::vector<intcoord> groundplan;	// Blocks (relative to to top left corner) taken up by the object
	
protected:
	// Properties
	bool hover;
};

#endif
