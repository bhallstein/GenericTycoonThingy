
#include "Unit.hpp"

Unit::Unit(NavMap *_navmap, int _x, int _y) : navmap(_navmap), destroyed(false) {
	dest_x = x = _x, dest_y = y = _y;
	a = b = 0;
	
	// Units’ responder block plan is just a single intcoord.
	intcoord c = {0,0};
	resp_blocks.push_back(c);
	
	state = S_IDLE;
	
	// Generate random destination
	dest_x = rand()%navmap->w, dest_y = rand()%navmap->h;
}

Unit::~Unit() {
	std::cout << "unit destruct" << std::endl;
}

void Unit::receiveEvent(Event *ev) {
	// Event handling shizzle
}

char Unit::col() {
	if (state == S_IDLE) return 'r';
	else if (state == S_TRAVELING) return 'b';
	else return 'w';
}

void Unit::update() {
	bool at_dest = (x == dest_x && y == dest_y);
	
	if (state == S_IDLE) {
		if (!at_dest)
			setToTraveling();
	}
	else if (state == S_WAITING) {
		frames_waited++;
		if (frames_waited == 60)
			frames_waited = 0, setToTraveling();
	}
	else if (at_dest)
		setToIdle();
	else if (state == S_TRAVELING) {
		if (route.empty())
			setToTraveling();
	}

	if (state != S_TRAVELING) return;	

	incrementLocation();
}

void Unit::nextInRoute() {
	route.erase(route.begin());
}
void Unit::setToIdle() {
	route.clear();
	state = S_IDLE;
}
void Unit::setToTraveling() {
	if (navmap->getRoute(x, y, dest_x, dest_y, &route))
		route.erase(route.begin()), state = S_TRAVELING;
	else
		setToWaiting();
}
void Unit::setToWaiting() {
	frames_waited = 0;
	state = S_WAITING;
}
void Unit::incrementLocation() {
	// Increment a & b toward next in route
	float step = 0.05;
	float a_diff = 0, b_diff = 0;

	int next_x = route[0]->x, next_y = route[0]->y;
	bool diagonal = (x != next_x && y != next_y);
	
	if (x < next_x) 		a_diff = step;
	else if (x > next_x) 	a_diff = -step;
	if (y < next_y) 		b_diff = step;
	else if (y > next_y) 	b_diff = -step;

	if (diagonal) a_diff *= 0.71, b_diff *= 0.71;	// For diagonal traveling, normalise the motion vector by dividing by √2

	a += a_diff, b += b_diff;
	
	// Check if we’ve reached the next loc. This happens:
	//   - for diagonals, when a^2 + b^2 = 2
	//   - for linears, when |a| = 1 or |b| = 1
	bool reached_next = ((diagonal && a*a + b*b >= 2) || (!diagonal && (a*a >= 1 || b*b >= 1)));
	if (reached_next) {
		// If on point of entering next loc, check is passable
		if (route[0]->passable) {
			x = next_x, y = next_y;
			a = b = 0;
			nextInRoute();
		}
		// If access denied, go back to the previous square. Waiting & recalculation will ensue upon arrival.
		else {
			route.clear();
			route.push_back( navmap->nodeAt(x, y) );
			x = next_x, y = next_y;
			a = b = 0;
		}
	}
}
