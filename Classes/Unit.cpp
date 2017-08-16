#include "Unit.hpp"
#include "NavMap.hpp"
#include "LuaHelper.hpp"
#include "Building.hpp"
#include "Level.hpp"
#include "Furnishing.hpp"

std::map<std::string, struct unitInfo> Unit::unitTypes;
std::string Unit::defaultColour;
std::string Unit::defaultHoverColour;
bool Unit::initialized = false;

Unit::Unit(ResponderMap *_rm, NavMap *_navmap, const char *_type, Level *_level, bool _placeableMode) :
	MappedObj(_rm, _placeableMode), navmap(_navmap), type(_type), level(_level),
	hover(false), mode(IDLE)
{
	intcoord c = {0,0};
	groundplan.push_back(c);
	
	// Get properties for this unit type
	u_colour      = &Unit::unitTypes[type].col;
	u_hoverColour = &Unit::unitTypes[type].hoverCol;
}
Unit::~Unit()
{
	std::cout << "Unit destruct" << std::endl;
}

bool Unit::canPlace(int _x, int _y) {
	return navmap->isPassableAt(_x, _y);
}
void Unit::finalizePlacement() {
	if (placeableMode) {
		if (Building *b = level->buildingAtLocation(x, y))
			b->addStaff(this);
	}
}

void Unit::receiveEvent(Event *ev) {
	if (ev->type == Event::MOUSEMOVE)
		hover = true;
	else if (ev->type == Event::LEFTCLICK) {
		char s[100];
		sprintf(s, "\nUnit %p - x,y:%d,%d a,b:%.2f,%.2f dest:%d,%d mode:%s\n",
			this, x, y, a, b, dest_x, dest_y,
			mode == WAITING ? "WAITING" :
			mode == VOYAGING ? "VOYAGING" :
			mode == ANIMATING ? "ANIMATING" :
			mode == IDLE ? "IDLE" : "UNKNOWN"
		);
		std::cout << s << std::endl;
	}
}

const char * Unit::col() {
	if (hover) { hover = false; return u_hoverColour->c_str(); }
	return u_colour->c_str();
}

void Unit::update() {
	if (hover) return;
	else if (mode == IDLE) { }
	else if (mode == WAITING && ++frames_waited >= 60) voyage(dest_x, dest_y);
	else if (mode == VOYAGING) { if (!incrementLocation()) wait(); }
	else if (mode == ANIMATING) incrementAnimation();
}

/*** Utility methods ***/

void Unit::getDespawnPoint(int *x, int *y) {
	int w = navmap->w, h = navmap->h, n = W::randUpTo(2*w + 2*h - 4);
	if (n < w)            *x = n,     *y = 0;
	else if (n < 2*w)     *x = n - w, *y = h - 1;
	else if (n < 2*w + h) *x = 0,     *y = n - 2*w;
	else                  *x = w - 1, *y = n - (2*w + h);
}
void Unit::despawn() {
	destroyed = true;
}
bool Unit::voyage(int _x, int _y) {
  	arrived = false;
	bool success = navmap->getRoute(x, y, dest_x = _x, dest_y = _y, &route);
	if (success) mode = VOYAGING;
	return success;
}
void Unit::runAnimation(/* ... */) {
	frames_animated = 0;
	animation_finished = false;
	mode = ANIMATING;
}
void Unit::incrementAnimation() {
	int length = 60;
	rotation += 3;
	if (++frames_animated >= length) {
		animation_finished = true;
		rotation = 0;
		mode = IDLE;
	}
}

void Unit::wait() {
	frames_waited = 0;
	mode = WAITING;
}

bool Unit::incrementLocation() {
	if (atDest()) {
		arrived = true;
		mode = IDLE;
		return true;
	}
	
	float step = 0.10;
	float a_diff = 0, b_diff = 0;
	bool diagonal = x != prev_x && y != prev_y;
	
	// Check if we've reached the next loc. This happens
	//  - for linears, when |a| < step or |b| < step
	//  - for diagonals, when a^2 + b^2 < 2 * step^2
	float t = a != 0 ? a : b;
	bool reached_next = (diagonal && a*a + b*b < 2*step*step) || (!diagonal && t*t < step*step);
	if (reached_next) {
		if (route.empty())
			a = b = 0;		// Arrived!
		else {
			// Get next in route
			int c = route[0]->x, d = route[0]->y;
			if (navmap->isPassableAt(c, d)) {
				prev_x = x, prev_y = y;
				x = c, y = d;
				a = prev_x == x ? 0 : prev_x < x ? -1 : 1;
				b = prev_y == y ? 0 : prev_y < y ? -1 : 1;
				route.erase(route.begin());
			}
			else {
				route.clear();
				return navmap->getRoute(x, y, dest_x, dest_y, &route);
			}
		}
	}
	else {
		// Decrement a & b toward current loc
		if (x > prev_x)      a_diff = step;
		else if (x < prev_x) a_diff = -step;
		if (y > prev_y)      b_diff = step;
		else if (y < prev_y) b_diff = -step;
		if (diagonal) a_diff *= 0.71, b_diff *= 0.71;	// For diagonal traveling, normalise the motion vector by dividing by √2
		a += a_diff, b += b_diff;
	}
	return true;
}
inline bool Unit::atDest() {
	return dest_x == x && dest_y == y && a == 0 && b == 0;
}
inline bool Unit::inHinterland() {
	return (
		x < HINTERLAND_WIDTH || x >= navmap->w-HINTERLAND_WIDTH || 
		y < HINTERLAND_WIDTH || y >= navmap->h-HINTERLAND_WIDTH
	);
}

bool Unit::initialize(W *_W) {
	if (Unit::initialized) return true;
	
	W::log("  Unit::initialize() called...");
	LuaHelper mrLua(_W);
	
	std::string path = _W->resourcesPath;
	path.append("units.lua");
	if (!mrLua.loadFile(path.c_str())) {
		W::log("Could not read units.lua");
		return false;
	}
	lua_State *L = mrLua.LuaInstance;
	
	try {
		// Set Unit defaults
		Unit::defaultColour      = mrLua.getvalue<const char *>("defaultColour");
		Unit::defaultHoverColour = mrLua.getvalue<const char *>("defaultHoverColour");
	} catch (MsgException &exc) {
		std::string s = "In units.lua, could not get defaults. Error: "; s.append(exc.msg);
		W::log(s.c_str());
	}
	
	// Construct Unit::unitTypes map
	if (!mrLua.pushtable("unitTypes")) {
		W::log("In units.lua, could not get the unitTypes table.");
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

		lua_getfield(L, -1, "hireCost");			// S: -1 colour; 2 subtable; -3 key; -4 table
		uInfo->hireCost = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : 0;

		lua_pop(L, 2);								// S: -1 key; -2 table
	}
	
	Unit::initialized = true;
	return true;
}
int Unit::getUnitHireCost(std::string _unitKey) {
	return unitTypes[_unitKey].hireCost;
}
