#include "Unit.hpp"

Unit::Unit(NavMap *_navmap, int _x, int _y) : navmap(_navmap), destroyed(false) {
	x = _x, y = _y;
	w = h = 1;
	a = b = 0;
	
	state = "idle";
	
	// Generate random destination
	goSomewhere(rand()%navmap->w, rand()%navmap->h);
}

Unit::~Unit() {

}

void Unit::receiveEvent(sf::Event *ev, EventResponder **p_e_r) {
	// Event handling shizzle
}

char Unit::col() {
	if (state == "idle") return 'r';
	else if (state == "travelling") return 'b';
	else return 'w';
}

void Unit::goSomewhere(int _destX, int _destY) {
	std::cout << "(dest: " << _destX << "," << _destY << ") ";
	destX = _destX, destY = _destY;
	
	std::cout << "getting route..." << std::endl;
	std::vector<NavNode*> route;
	bool success = navmap->getRoute(x, y, destX, destY, &route);
	std::cout << (success ? "got route:" : "no route possible :(") << std::endl;
	
	for (int i=0; i < route.size(); i++)
		std::cout << " " << route[i]->x << "," << route[i]->y << std::endl;
}