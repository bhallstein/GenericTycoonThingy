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
	void loadFromSFEvent(sf::Event *sf_event, int block_width, int block_height);
	
	// Properties
	dbt_event type;
	dbt_event key;
	
	int x, y;		// Block coords
	float a, b;		// Float coords (offset)
	
protected:
	
	// Methods
	void setCoords(int pixel_x, int pixel_y, int block_width, int block_height);
	
};

dbt_event keyFromSFKeyCode(sf::Keyboard::Key sf_key_code);

#endif
