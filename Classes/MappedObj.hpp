/*
 * MappedObj.hpp - base class for objects to be drawn on the map
 *
 */

#ifndef MAPPEDOBJ_H
#define MAPPEDOBJ_H

#include <vector>

#include "types.hpp"
#include "EventResponder.hpp"

class MappedObj : public EventResponder
{
public:
	MappedObj(int _x, int _y) : a(0), b(0) { x = _x, y = _y; }
	~MappedObj() { }
	
	// Methods
	virtual void update() = 0;			// TLOs will generally want to handle updates
	virtual const char * col() = 0;		// Temporary: override to return colour the object should be drawn
	
	// Properties
	float a, b;							// Floating pt offset from integer location (default 0)
	std::vector<intcoord> ground_plan;	// Blocks (relative to to top left corner) taken up by the object
	
protected:
	// Methods
	void setGroundPlan(intcoord c[]) {						// Convenience function for setting an object’s ground plan.
		ground_plan.clear();								// IMPORTANT: terminate the array of intcoords with {-1,-1}.
		for (int i=0; c[i].x >= 0; i++) ground_plan.push_back(c[i]);
	}
	
	// Properties
	bool hover;
};

#endif
