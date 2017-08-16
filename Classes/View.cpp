#include "View.hpp"
#include "Event.hpp"

View::View(W *_theW, JenniferAniston &_aniston) : theW(_theW), jenniferAniston(_aniston)
{
	updatePosition();
}

View::~View()
{
	std::cout << "view destruct" << std::endl;
}

void View::updatePosition() {
	jenniferAniston.refresh();
	x = jenniferAniston.x;
	y = jenniferAniston.y;
	width  = jenniferAniston.width;
	height = jenniferAniston.height;
}

void View::_draw() {
	theW->setUpDrawingForView(this);
	draw();
}

void View::draw() {
	// Everybody loves a checkerboard pattern
	for (int i=0; i < width; i += 20)
		for (int j=0; j < height; j += 20)
			theW->drawRect(
				i, j, 20, 20,
				(j/20)%2
					? ((i/20)%2 ? "white" : "black")
					: ((i/20)%2 ? "black" : "white")
			);
}

void View::receiveEvent(Event *ev) {
	ev->x -= x, ev->y -= y;
	processMouseEvent(ev);
}

void View::processMouseEvent(Event *ev) {
	// Hello, event. Want to be friends?
}
