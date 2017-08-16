#include "Unit.hpp"

#include "LuaHelper.hpp"
#include "Building.hpp"
#include "Level.hpp"
#include "Furnishing.hpp"
#include "MrPaths.hpp"
#include "W.h"

W::EventType::T W::EventType::INTERRUPT_UNITPICKUP = W::Event::registerType();

std::map<std::string, struct unitInfo> Unit::unitTypes;
W::Colour::T Unit::defaultColour;
W::Colour::T Unit::defaultHoverColour;
bool Unit::initialized = false;

Unit::Unit(W::EventHandler *_eh, W::NavMap *_navmap, const char *_type, Level *_level, bool _placeableMode) :
	PlaceableManager(_eh, _placeableMode), navmap(_navmap), type(_type), level(_level),
	contextBuilding(NULL), mode(IDLE), hired(false)
{
	plan.resize(1);
	W::rect *r = &plan[0];
	r->sz.width = r->sz.height = 1;
	
	// Get properties for this unit type
	unitInfo *t = &unitTypes[type];
	u_colour               = t->col;
	u_hoverColour          = t->hoverCol;
	u_compatibleBehaviours = &t->compatibleBehaviours;
	u_isStaff              = t->isStaff;
	u_hireCost             = t->hireCost;
}
Unit::~Unit()
{
	std::cout << "unit destruct" << std::endl;
}

bool Unit::canPlace(int _x, int _y) {
	return navmap->isPassableAt(_x, _y);
}
void Unit::finalizePlacement() {
	if (placeableMode && u_isStaff) {
		if (Building *b = level->buildingAtLocation(pos.x, pos.y))
			b->addStaff(this), contextBuilding = b;
		if (!hired) {
			level->chargePlayer(Unit::hireCostForType(type.c_str()));
			hired = true;
		}
	}
	using namespace W::EventType;
	eh->subscribeToEventType(LEVEL_LEFTMOUSEDOWN, new W::Callback(&Unit::receiveEvent, this));
	eh->subscribeToEventType(LEVEL_LEFTMOUSEUP, new W::Callback(&Unit::receiveEvent, this));
	eh->subscribeToEventType(LEVEL_RIGHTMOUSEDOWN, new W::Callback(&Unit::receiveEvent, this));
	eh->subscribeToEventType(LEVEL_RIGHTMOUSEUP, new W::Callback(&Unit::receiveEvent, this));
	eh->subscribeToEventType(LEVEL_MOUSEMOVE, new W::Callback(&Unit::receiveEvent, this));

	prev_x = pos.x, prev_y = pos.y;
}

void Unit::receiveEvent(W::Event *ev) {
	using namespace W::EventType;

	if (mode == ANIMATING)
		return;

	if (ev->type == LEVEL_MOUSEMOVE)
		hover = true;
	else if (ev->type == LEVEL_LEFTMOUSEDOWN)
		printDebugInfo();
	else if (ev->type == LEVEL_RIGHTMOUSEDOWN)
		if (u_isStaff) pickUp();
}

W::Colour::T Unit::col() {
	if (hover) { hover = false; return u_hoverColour; }
	return u_colour;
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
void Unit::destroy() {
	destroyed = true;
}
bool Unit::voyage(int _x, int _y) {
  	arrived = false;
	bool success = navmap->getRoute(pos.x, pos.y, dest_x = _x, dest_y = _y, &route);
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

void Unit::pickUp() {
	W::Event ev;
	ev.setType(W::EventType::INTERRUPT_UNITPICKUP);
	ev._payload = this;
	eh->dispatchEvent(&ev);
	if (contextBuilding) contextBuilding->removeStaff(this);
	contextBuilding = NULL;
	PlaceableManager::pickUp();
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
	bool diagonal = pos.x != prev_x && pos.y != prev_y;
	
	// Check if we've reached the next loc. This happens
	//  - for linears, when |a| < step or |b| < step
	//  - for diagonals, when a^2 + b^2 < 2 * step^2
	float t = pos.a != 0 ? pos.a : pos.b;
	bool reached_next = (diagonal && pos.a*pos.a + pos.b*pos.b < 2*step*step) || (!diagonal && t*t < step*step);
	if (reached_next) {
		if (route.empty())
			pos.a = pos.b = 0;		// Arrived!
		else {
			// Get next in route
			int c = route[0].x, d = route[0].y;
			if (navmap->isPassableAt(c, d)) {
				prev_x = pos.x, prev_y = pos.y;
				pos.x = c, pos.y = d;
				pos.a = prev_x == pos.x ? 0 : prev_x < pos.x ? -1 : 1;
				pos.b = prev_y == pos.y ? 0 : prev_y < pos.y ? -1 : 1;
				route.erase(route.begin());
			}
			else
				return navmap->getRoute(pos.x, pos.y, dest_x, dest_y, &route);
		}
	}
	else {
		// Decrement a & b toward current loc
		if (pos.x > prev_x)      a_diff = step;
		else if (pos.x < prev_x) a_diff = -step;
		if (pos.y > prev_y)      b_diff = step;
		else if (pos.y < prev_y) b_diff = -step;
		if (diagonal) a_diff *= 0.71, b_diff *= 0.71;	// For diagonal traveling, normalise the motion vector by dividing by √2
		pos.a += a_diff, pos.b += b_diff;
	}
	return true;
}
inline bool Unit::atDest() {
	return dest_x == pos.x && dest_y == pos.y && pos.a == 0 && pos.b == 0;
}
inline bool Unit::inHinterland() {
	return (
		pos.x < HINTERLAND_WIDTH || pos.x >= navmap->w-HINTERLAND_WIDTH ||
		pos.y < HINTERLAND_WIDTH || pos.y >= navmap->h-HINTERLAND_WIDTH
	);
}

std::vector<std::string>* Unit::getCompatibleBehaviours() {
	return u_compatibleBehaviours;
}

bool Unit::initialize() {
	if (Unit::initialized) return true;
	
	W::log << "  Unit::initialize() called..." << std::endl;
	LuaHelper mrLua;
	
	std::string path = MrPaths::luaPath;
	path.append("units.lua");
	if (!mrLua.loadFile(path.c_str())) {
		W::log << "Could not read units.lua" << std::endl;
		return false;
	}
	lua_State *L = mrLua.LuaInstance;
	
	try {
		// Set Unit defaults
		Unit::defaultColour      = strToColour(mrLua.getvalue<const char *>("defaultColour"));
		Unit::defaultHoverColour = strToColour(mrLua.getvalue<const char *>("defaultHoverColour"));
	} catch (W::Exception &exc) {
		std::string s = "In units.lua, could not get defaults. Error: "; s.append(exc.msg);
		W::log << s << std::endl;
	}
	
	// Construct Unit::unitTypes map
	if (!mrLua.pushtable("unitTypes")) {
		W::log << "In units.lua, could not get the unitTypes table." << std::endl;
		return false;
	}
	lua_pushnil(L);									// S: -1 nil; -2 table
	while (lua_next(L, -2) != 0) {					// S: -1 subtable; -2 key; -3 table
		if (!lua_istable(L, -1)) {			// If not a subtable, skip
			lua_pop(L, 1);
			continue;
		}
		const char *uType = lua_tostring(L, -2);
		struct unitInfo *uInfo = &Unit::unitTypes[uType];
		
		lua_getfield(L, -1, "colour");				// S: -1 colour; -2 subtable; -3 key; -4 table
		if (lua_isstring(L, -1))
			uInfo->col = strToColour(lua_tostring(L, -1));
		else
			uInfo->col = Unit::defaultColour;
		lua_pop(L, 1);
		
		lua_getfield(L, -1, "hoverColour");			// S: -1 colour; -2 subtable; -3 key; -4 table
		if (lua_isstring(L, -1))
			uInfo->hoverCol = strToColour(lua_tostring(L, -1));
		else
			uInfo->hoverCol = Unit::defaultHoverColour;
		lua_pop(L, 1);
		
		lua_getfield(L, -1, "isStaff");				// S: -1 isStaff; -2 subtable; -3 key; -4 table
		uInfo->isStaff = lua_isboolean(L, -1) ? lua_toboolean(L, -1) : false;
		lua_pop(L, 1);
		
		lua_getfield(L, -1, "hireCost");			// S: -1 colour; 2 subtable; -3 key; -4 table
		uInfo->hireCost = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : 0;
		lua_pop(L, 1);								// S: -1 subtable; -2 key; -3 table

		// Compatible Behaviours
		if (!mrLua.pushSubtable("compatibleBehaviours")) {
			char s[100]; sprintf(s, "In units.lua, could not find compatibleBehaviours for '%s' type", uType);
			W::log << s << std::endl;
			return false;
		}
		std::string s = "    "; s += uType; s += "\n      compatibleBehaviours: ";
		int n = 0;
		lua_pushnil(L);								// S: -1 nil; -2 compatibleBehaviours; -3 subtable; ...
		while (lua_next(L, -2)) {					// S: -1 interaction, -2 key; -3 compatibleBehaviours; ...
			if (!lua_isstring(L, -1)) {
				lua_pop(L, 1);
				continue;
			}
			std::string iType = lua_tostring(L, -1);
			uInfo->compatibleBehaviours.push_back(iType);
			n++;
			s += iType; s += ", ";
			lua_pop(L, 1);							// S: -1 key; -2 compatibleBehaviours
		}											// S: -1 compatibleBehaviours; -2 subtable; -3 key; -4 table
		lua_pop(L, 1);								// S: -1 subtable; -2 key; -3 table
		if (n > 0) s.erase(s.size() - 2);
		else s += "none";
		W::log << s << std::endl;
		lua_pop(L, 1);								// S: -1 key; -2 table
	}
	
	Unit::initialized = true;
	return true;
}
int Unit::hireCostForType(const char *_type) {
	return unitTypes[_type].hireCost;
}

void Unit::printDebugInfo() {
	printf("\nUnit %p - x,y:%d,%d a,b:%.1f,%.1f dest:%d,%d mode:%s\n\n",
		this, pos.x, pos.y, pos.a, pos.b, dest_x, dest_y,
		mode == WAITING ? "WAITING" :
		mode == VOYAGING ? "VOYAGING" :
		mode == ANIMATING ? "ANIMATING" :
		mode == IDLE ? "IDLE" : "UNKNOWN"
	);
}
