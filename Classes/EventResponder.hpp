/*
 * EventResponder.hpp - abstract base class for event responders
 *
 */

#ifndef EVENTRESPONDER_H
#define EVENTRESPONDER_H

#include "Event.hpp"

class EventResponder
{
public:
	EventResponder() { };
	~EventResponder() { };

	virtual void receiveEvent(Event *) = 0;
	
	int x, y, width, height;		// For ERs that need to handle mouse events
	
};

#endif
