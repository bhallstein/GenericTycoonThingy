/*
 * EventResponder.h
 *
 * An abstract class, for subclassing, to allow objects to handle events.
 * To define a subclass’s event behaviours, override receiveEvent().
 *
 */

#ifndef EVENTRESPONDER_H
#define EVENTRESPONDER_H

#include <SFML/Window.hpp>

#include "Event.h"

class EventResponder
{
public:
	EventResponder();
	~EventResponder();
	
	virtual void receiveEvent(Event *ev, EventResponder **p_e_r);

protected:
	
};

#endif