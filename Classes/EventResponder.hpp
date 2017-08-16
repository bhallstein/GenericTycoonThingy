/*
 * EventResponder.h - abstract base class for memory mapped event responders
 *
 */

#ifndef EVENTRESPONDER_H
#define EVENTRESPONDER_H

#include <SFML/Window.hpp>

#include "types.h"
#include "Event.hpp"

class EventResponder
{
public:
	EventResponder() { };
	~EventResponder() { };

	// Methods
	virtual void receiveEvent(Event *) = 0;
	virtual char col() = 0;
	
	// Properties
	int x, y;							// Coordinates of top left corner
	std::vector<intcoord> resp_blocks;	// List of blocks (relative to to top left corner) considered part of the responder

protected:
	
};

#endif
