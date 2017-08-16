#include "Unit.hpp"
#include "NavMap.hpp"
#include "LuaHelper.hpp"
#include "Building.hpp"
#include "Level.hpp"

std::map<std::string, struct unitInfo> Unit::unitTypes;
std::string Unit::defaultColour;
std::string Unit::defaultHoverColour;

Unit::Unit(ResponderMap *_rm, NavMap *_navmap, const char *_type, Level *_level) :
	MappedObj(_rm, false), navmap(_navmap), type(_type), level(_level),
	hover(false), stage(0)
{
	intcoord c = {0,0};
	groundplan.push_back(c);
	
	// Get properties for this unit type
	u_colour      = &Unit::unitTypes[type].col;
	u_hoverColour = &Unit::unitTypes[type].hoverCol;
}
Unit::~Unit() { }

bool Unit::canPlace(int _x, int _y) {
	return navmap->isPassableAt(_x, _y);
}

void Unit::receiveEvent(Event *ev) {
	if (ev->type == Event::MOUSEMOVE)
		hover = true;
}

const char * Unit::col() {
	if (hover) { hover = false; return u_hoverColour->c_str(); }
	return u_colour->c_str();
}

void Unit::addIntention(Intention::Enum _intention) {
	intentions.push_back(_intention);
}

void Unit::update() {
	if (hover) return;
	Intention::Enum curIntention = intentions.empty() ? Intention::NONE : intentions[0];
	if (curIntention == Intention::HAIRCUT)       intentionUpdate_Haircut();
	else if (curIntention == Intention::PIE)      intentionUpdate_Pie();
	else if (curIntention == Intention::DESTRUCT) intentionUpdate_Destruct();
}

void Unit::intentionUpdate_Haircut() {
	if (stage == -1) {		// Waiting
		if (++frames_waited >= 60) stage++;
	}
	else if (stage == 0) {	// Route to barber shop
		dest_building = level->randomBuildingWithType("barber");
		if (dest_building == NULL)
			frames_waited = 0, stage = -1;
		else {
			dest_building->getEntryPoint(&dest_x, &dest_y);
			if (navmap->getRoute(x, y, dest_x, dest_y, &route))
				stage++;
			else
				frames_waited = 0, stage = -1;
		}
	}
	else if (stage == 1) {	// Move to destination
		if (atDest()) stage++;
		else if (!incrementLocation()) stage = 0;
	}
	else if (stage == 2) {	// Entry negotiation
		dest_building->getQueuePoint(&dest_x, &dest_y);
		stage += 2;
	}
	else if (stage == 3) {	// Waiting
		if (++frames_waited >= 60) stage++;
	}
	else if (stage == 4) {	// Route to queue point
		if (navmap->getRoute(x, y, dest_x, dest_y, &route))
			stage++;
		else
			frames_waited = 0, stage = 3;
	}
	else if (stage == 5) {	// Move to queue point
		if (atDest()) frames_waited = 0, stage++;
		else if (!incrementLocation()) stage = 4;
	}
	else if (stage == 6) {	// Queue
		if (++frames_waited == 60) nextIntention();
	}
}
void Unit::intentionUpdate_Pie() {
	if (stage == -1) {		// Waiting
		if (++frames_waited >= 60) stage++;
	}
	else if (stage == 0) {	// Route to barber shop
		dest_building = level->randomBuildingWithType("pieshop");
		if (dest_building == NULL)
			frames_waited = 0, stage = -1;
		else {
			dest_building->getEntryPoint(&dest_x, &dest_y);
			if (navmap->getRoute(x, y, dest_x, dest_y, &route))
				stage++;
			else
				frames_waited = 0, stage = -1;
		}
	}
	else if (stage == 1) {	// Move to destination
		if (atDest()) stage++;
		else if (!incrementLocation()) stage = 0;
	}
	else if (stage == 2) {	// Entry negotiation
		dest_building->getQueuePoint(&dest_x, &dest_y);
		stage += 2;
	}
	else if (stage == 3) {	// Waiting
		if (++frames_waited >= 60) stage++;
	}
	else if (stage == 4) {	// Route to queue point
		if (navmap->getRoute(x, y, dest_x, dest_y, &route))
			stage++;
		else
			frames_waited = 0, stage = 3;
	}
	else if (stage == 5) {	// Move to queue point
		if (atDest()) frames_waited = 0, stage++;
		else if (!incrementLocation()) stage = 4;
	}
	else if (stage == 6) {	// Queue
		if (++frames_waited == 60) nextIntention();
	}
}
void Unit::intentionUpdate_Destruct() {
	if (stage == -1) {		// Waiting
		if (++frames_waited == 60) stage++;
	}
	else if (stage == 0) {	// Find route to despawn loc
		switch(W::randUpTo(4))
		{
			case 0: //head left
				dest_x = 0, dest_y = W::randUpTo(navmap->h);
				break;
			case 1: //head right
				dest_x = navmap->w-1, dest_y = W::randUpTo(navmap->h);
				break;
			case 2: //head up
				dest_y = 0, dest_x = W::randUpTo(navmap->w);
				break;
			case 3: //head down
				dest_y = navmap->h-1, dest_x = W::randUpTo(navmap->w);
				break;
		}
		if (navmap->getRoute(x, y, dest_x, dest_y, &route))
			stage++;
		else
			frames_waited = 0, stage = -1;
	}
	else if (stage == 1) {	// Move to despawn loc
		if (atDest()) stage++;
		else if (!incrementLocation()) stage = 0;
	}
	else if (stage == 2) {	// Despawn
		destroyed = true;
	}
}
void Unit::nextIntention() {
	stage = 0;
	if (!intentions.empty()) intentions.erase(intentions.begin());
}


bool Unit::incrementLocation() {
	if (atDest()) return true;
	
	float step = 0.05;
	float a_diff = 0, b_diff = 0;
	bool diagonal = x != prev_x && y != prev_y;
	
	// Check if we've reached the next loc. This happens
	//  - for linears, when |a| < step or |b| < step
	//  - for diagonals, when a^2 + b^2 < 2 * step^2
	float t = a != 0 ? a : b;
	bool reached_next = (diagonal && a*a + b*b < 2*step*step) || (!diagonal && t*t < step*step);
	if (reached_next) {
		if (route.empty())
			a = b = 0;			// Arrived!
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
				return false;
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
	W::log("Unit::initialize() called...");
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
		Unit::defaultColour           = mrLua.getvalue<const char *>("defaultColour");
		Unit::defaultHoverColour      = mrLua.getvalue<const char *>("defaultHoverColour");
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
		
		lua_pop(L, 2);								// S: -1 key; -2 table
	}
	
	W::log("...initialization succeeded.");
	return true;
}
