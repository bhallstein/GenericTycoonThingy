/*
 * Button.hpp
 *
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <iostream>

#include "types.hpp"
#include "Event.hpp"

class Button {
public:
	Button(int _x, int _y, int _w, int _h, const char *_action);
	~Button();

	// Methods
	void setHover();
	const char * col();
	Event* getEvent();
	
	// Properties
	int x, y, width, height;

protected:
	// Properties
	bool hover;
	Event event;
};

#endif
