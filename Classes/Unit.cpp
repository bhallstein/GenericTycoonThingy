#include "Unit.hpp"
#include "NavMap.hpp"
#include "LuaHelper.hpp"

std::map<std::string, struct unitInfo> Unit::unitTypes;
std::string Unit::defaultColour;
std::string Unit::defaultHoverColour;
std::string Unit::defaultColourWhenMoving;

Unit::Unit(NavMap *_navmap, int _x, int _y, const char *_type) :
	MappedObj(_x, _y),
	navmap(_navmap),
	destroyed(false),
	dest_x(_x),
	dest_y(_y),
	type(_type),
	state(S_IDLE),
	hover(false)
{
	std::vector<intcoord> p;
	intcoord c = {0,0}; p.push_back(c);
	setGroundPlan(&p);
	
	dest_x = rand()%navmap->w, dest_y = rand()%navmap->h;	// Generate random destination
	
	// Get properties for this unit type
	u_colour           = Unit::unitTypes[type].col;
	u_hoverColour      = Unit::unitTypes[type].hoverCol;
	u_colourWhenMoving = Unit::unitTypes[type].colWhenMoving;
}
Unit::~Unit() { }

void Unit::receiveEvent(Event *ev) {
	if (ev->type == Event::MOUSEMOVE)
		hover = true;
}

const char * Unit::col() {
	if (hover) { hover = false; return u_hoverColour.c_str(); }
	else if (state == S_IDLE) return u_colour.c_str();
	else if (state == S_TRAVELING) return u_colourWhenMoving.c_str();
	else return "red";
}

void Unit::update() {
	if (hover) return;
	
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

bool Unit::initialize(W *_W) {
	W::log("Unit::initialize() called...");
	
	LuaHelper mrLua(_W);
	
	std::string path = _W->resourcesPath;
	path.append("units.lua");
	if (!mrLua.loadFile(path.c_str())) {
		W::log("Could not read units.lua");
		return false;
	}
	lua_State *L = mrLua.LuaInstance;
	
	// Set Unit::defaultColour
	lua_getglobal(L, "defaultColour");				// S: -1 value
	if (!lua_isstring(L, -1)) {
		W::log("In units.lua, could not find defaultColour");
		return false;
	}
	Unit::defaultColour = lua_tostring(L, -1);
	lua_pop(L, 1);									// S: ~
	
	// Set Unit::defaultHoverColour
	lua_getglobal(L, "defaultHoverColour");			// S: -1 value
	if (!lua_isstring(L, -1)) {
		W::log("In units.lua, could not find defaultHoverColour");
		return false;
	}
	Unit::defaultHoverColour = lua_tostring(L, -1);
	lua_pop(L, 1);									// S: ~
	
	// Set Unit::defaultColourWhenMoving
	lua_getglobal(L, "defaultColourWhenMoving");	// S: -1 value
	if (!lua_isstring(L, -1)) {
		W::log("In units.lua, could not find defaultColourWhenMoving");
		return false;
	}
	Unit::defaultColourWhenMoving = lua_tostring(L, -1);
	lua_pop(L, 1);									// S: ~	
	
	// Construct Unit::unitTypes map
	if (!mrLua.pushtable("unitTypes")) {
		W::log("In units.lua, could not push the unitTypes table onto the stack");
		return false;
	}
	lua_pushnil(L);									// S: -1 nil; -2 table
	while (lua_next(L, -2) != 0) {					// S: -1 subtable; -2 key; -3 table
		if (!lua_istable(L, -1)) {			// If not a subtable, skip
			lua_pop(L, 1);
			continue;
		}
		const char *u_type = lua_tostring(L, -2);
		struct unitInfo *uInfo = &Unit::unitTypes[u_type];
		
		lua_getfield(L, -1, "colour");				// S: -1 colour; -2 subtable; -3 key; -4 table
		uInfo->col = lua_isstring(L, -1) ? lua_tostring(L, -1) : Unit::defaultColour;
		lua_pop(L, 1);
		
		lua_getfield(L, -1, "hoverColour");			// S: -1 colour; -2 subtable; -3 key; -4 table
		uInfo->hoverCol = lua_isstring(L, -1) ? lua_tostring(L, -1) : Unit::defaultHoverColour;
		lua_pop(L, 1);
		
		lua_getfield(L, -1, "colourWhenMoving");	// S: -1 colour; 2 subtable; -3 key; -4 table
		uInfo->colWhenMoving = lua_isstring(L, -1) ? lua_tostring(L, -1) : Unit::defaultColourWhenMoving;
		
		lua_pop(L, 2);								// S: -1 key; -2 table
	}
	
	W::log("...initialization succeeded.");
	return true;
}
