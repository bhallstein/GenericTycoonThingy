/*
 * EventResponder.h
 * - an abstract class, for subclassing, to allow various objects to handle events.
 * - implement the subclass’s specific event behaviour by overriding receiveEvent().
 *
 */

#ifndef EVENTRESPONDER_H
#define EVENTRESPONDER_H

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

class EventResponder
{
public:
	EventResponder();
	~EventResponder();
	
	virtual void receiveEvent(sf::Event *ev, EventResponder **p_e_r);

protected:
	
};

#endif