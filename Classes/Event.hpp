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
	void loadFromMousePos(sf::Vector2i);
	void convertCoords(int block_width, int block_height, int offset_x = 0, int offset_y = 0);
	
	// Properties
	dbt_event_code type;
	dbt_event_code key;
	
	int x, y;		// Block coords
	float a, b;		// Float coords (offset)
	
protected:
	
};

dbt_event_code keyFromSFKeyCode(sf::Keyboard::Key sf_key_code);

#endif
