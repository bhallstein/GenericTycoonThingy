#include "Button.hpp"

Button::Button(int _x, int _y, int _width, int _height, const char *_action) :
	x(_x), y(_y), width(_width), height(_height), hover(false)
{
	event.setType(Event::BUTTONCLICK);
	event.setPayload(_action);
}
Button::~Button()
{
	std::cout << "Button destruct" << std::endl;
}

void Button::setHover() {
	hover = true;
}
const char * Button::col() {
	if (hover) { hover = false; return "red"; }
	return "black";
}
Event* Button::getEvent() {
	return &event;
}
