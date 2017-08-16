#include "Button.hpp"
#include "ButtonReceiver.hpp"

Button::Button(ButtonReceiver *_receiver, int _x, int _y, int _width, int _height) :
	receiver(_receiver), hover(false)
{
	x = _x, y = _y;
	width = _width, height = _height;
}
Button::~Button() {
	std::cout << "Button destruct" << std::endl;
}

void Button::receiveEvent(Event *ev) {
	if (ev->type == Event::MOUSEMOVE)
		hover = true;
	else if (ev->type == Event::LEFTCLICK)
		receiver->buttonClick(this);
}
colour_name Button::col() {
	if (hover) { hover = false; return RED; }
	return BLACK;
}
