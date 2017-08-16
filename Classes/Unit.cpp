#include "Unit.hpp"

Unit::Unit(NavMap *_navmap, int _x, int _y) : navmap(_navmap), destroyed(false) {
	x = _x, y = _y;
	w = h = 1;
	a = b = 0;
	
	state = S_IDLE;
	
	// Generate random destination
	goSomewhere(rand()%navmap->w, rand()%navmap->h);
}

Unit::~Unit() {

}

void Unit::receiveEvent(sf::Event *ev, EventResponder **p_e_r) {
	// Event handling shizzle
}

char Unit::col() {
	if (state == S_IDLE) return 'r';
	else if (state == S_TRAVELING) return 'b';
	else return 'w';
}

void Unit::goSomewhere(int _destX, int _destY) {
	std::cout << "(dest: " << _destX << "," << _destY << ") ";
	destX = _destX, destY = _destY;
	
	std::cout << "getting route..." << std::endl;
	bool success = navmap->getRoute(x, y, destX, destY, &route);
	if (success) {
		state = S_TRAVELING;
		std::cout << "traveling to destination" << std::endl;
	}
}

void Unit::nextInRoute() {
	route.erase(route.begin());
}

void Unit::update() {
	if (state == S_IDLE) return;
	if (x == route[0]->x && y == route[0]->y) {
		a = b = 0;
		nextInRoute();
		if (!route.size()) {
			state = S_IDLE;
			return;
		}
	}
	
	int next_x = route[0]->x, next_y = route[0]->y;
	float diff = 0.05;
	
	float a_diff = 0, b_diff = 0;
	if (x < next_x) a_diff = diff;
	else if (x > next_x) a_diff = -diff;
	if (y < next_y) b_diff = diff;
	else if (y < next_y) b_diff = -diff;

	if (a_diff != 0 and b_diff != 0)
		a_diff *= 0.71, b_diff *= 0.71;

	a += a_diff, b += b_diff;
	
	if (next_x - x != 0 && next_y - y != 0) {
		if (a*a + b*b >= 2)
			x = next_x, y = next_y, a = b = 0;
	}
	else if (a*a > 1 || b*b > 1)
		x = next_x, y = next_y, a = b = 0;
}



