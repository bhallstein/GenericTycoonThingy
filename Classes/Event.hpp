/*
 * Event.h
 *
 */

#ifndef EVENT_H
#define EVENT_H

#include <SFML/Window.hpp>

#include "types.h"


class Event
{
public:
	Event();
	~Event();
	
	// Methods
	void loadFromSFEvent(sf::Event *sf_event);
	void convertCoords(int block_width, int block_height);
	
	// Properties
	dbt_event_code type;
	dbt_event_code key;
	
	int x, y;		// Block coords
	float a, b;		// Float coords (offset)
	
protected:
	
};

dbt_event_code keyFromSFKeyCode(sf::Keyboard::Key sf_key_code);

#endif
