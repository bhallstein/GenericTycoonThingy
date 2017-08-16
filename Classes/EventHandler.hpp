/*
 * EventHandler.h
 *
 * EventHandler takes incoming events and hands them off to subscribed Views and EventResponders.
 *
 * Views receive all mouse events that occur over the top of them, via View::acceptEvent(Event *).
 * EventResponders receive the specific events they have subscribed to, via EventResponder::receiveEvent(Event *).
 * 
 * EventHandler does not implement any sort of privileged event response, which has settled into a new home in View.
 *
 */

#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <list>

// #include <SFML/Graphics.hpp>
// #include <SFML/Window.hpp>

#include "types.h"
#include "Event.hpp"
#include "EventResponder.hpp"
#include "View.hpp"


class EventHandler {
public:
	EventHandler();
	~EventHandler();
	
	// Methods
	void subscribe(View *);			// Subscribe a View to mouse events that occur over the top of it
	void unsubscribe(View *);
	
	void subscribe(EventResponder *, dbt_event_code);		// Subscribe a Responder to a specific type of event
	void unsubscribe(EventResponder *, dbt_event_code);		// (for responding to key presses, not mousey business)

	void dispatchEvent(Event *);

protected:
	// Properties
	std::list<View*> subscribed_views;
	std::map<dbt_event_code, std::list<EventResponder*> > keycode_subscribed_responders;
	
};


#endif
