/*
 * EventResponder.h - abstract base class for event responders
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

	virtual void receiveEvent(Event *) = 0;

protected:
	
};

#endif
