/*
 * Button.hpp - buttons
 *
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <iostream>

#include "types.hpp"
#include "EventResponder.hpp"

class Menu;
class ButtonReceiver;

class Button : public EventResponder
{
public:
	Button(ButtonReceiver *, int _x, int _y, int _width, int _height);
	~Button();

	// Methods
	void receiveEvent(Event *);
	void update() { }
	colour col();

protected:
	// Properties
	ButtonReceiver *receiver;
	bool hover;

};

#endif
