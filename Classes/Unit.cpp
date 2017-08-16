#include "Unit.hpp"

#include "LuaHelper.hpp"
#include "Building.hpp"
#include "Level.hpp"
#include "Furnishing.hpp"
#include "MrPaths.hpp"
#include "W.h"

W::EventType::T W::EventType::INTERRUPT_UNITPICKUP = W::Event::registerType();

std::map<std::string, struct unitInfo> Unit::unitTypes;
W::Colour Unit::defaultColour;
W::Colour Unit::defaultHoverColour;
bool Unit::initialized = false;

Unit::Unit(W::NavMap *_navmap, const char *_type, Level *_level, bool _placeableMode) :
	PlaceableManager(_placeableMode), navmap(_navmap), type(_type), level(_level),
	mode(IDLE), hired(false), skBehaviour(NULL)
{
	plan.resize(1);
	W::rect *r = &plan[0];
	r->sz.width = r->sz.height = 1;
	
	// Save ptr to properties for this unit type
	std::map<std::string, unitInfo>::iterator it = unitTypes.find(type);
	if (it == unitTypes.end()) {
		char s[200];
		sprintf(s, "Unit created of unknown type \"%s\"\n", _type);
		throw W::Exception(s);
	}
	uInfo = &it->second;
}
Unit::~Unit()
{
	std::cout << "unit destruct" << std::endl;
	using namespace W::EventType;
	W::Messenger::unsubscribeFromEventType(LEVEL_LEFTMOUSEDOWN,  this);
	W::Messenger::unsubscribeFromEventType(LEVEL_LEFTMOUSEUP,    this);
	W::Messenger::unsubscribeFromEventType(LEVEL_RIGHTMOUSEDOWN, this);
	W::Messenger::unsubscribeFromEventType(LEVEL_RIGHTMOUSEUP,   this);
	W::Messenger::unsubscribeFromEventType(LEVEL_MOUSEMOVE,      this);
	if (skBehaviour) skBehaviour->destroy();
}

bool Unit::canPlace(int _x, int _y) {
	return navmap->isPassableAt(_x, _y);
}
void Unit::finalizePlacement() {
	if (skBehaviour) skBehaviour->unitWasPutDown();
	if (uInfo->isStaff && placeableMode) {
		level->chargePlayer(uInfo->hireCost);
		hired = true;
	}
	
	using namespace W::EventType;
	W::Messenger::subscribeToEventType(LEVEL_LEFTMOUSEDOWN,  W::Callback(&Unit::receiveEvent, this));
	W::Messenger::subscribeToEventType(LEVEL_LEFTMOUSEUP,    W::Callback(&Unit::receiveEvent, this));
	W::Messenger::subscribeToEventType(LEVEL_RIGHTMOUSEDOWN, W::Callback(&Unit::receiveEvent, this));
	W::Messenger::subscribeToEventType(LEVEL_RIGHTMOUSEUP,   W::Callback(&Unit::receiveEvent, this));
	W::Messenger::subscribeToEventType(LEVEL_MOUSEMOVE,      W::Callback(&Unit::receiveEvent, this));

	prev_pos = pos;
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
		if (uInfo->isStaff) pickUp();
}

W::Colour& Unit::col() {
	if (hover) { hover = false; return uInfo->hoverCol; }
	return uInfo->col;
}

void Unit::update() {
	if (hover) return;
	else if (mode == IDLE) { }
	else if (mode == WAITING && ++frames_waited >= 60) voyage(dest);
	else if (mode == VOYAGING) { if (!incrementLocation()) wait(); }
	else if (mode == ANIMATING) incrementAnimation();
}

/*** Utility methods ***/

void Unit::getDespawnPoint(W::position &p) {
	int w = navmap->w, h = navmap->h, n = W::randUpTo(2*w + 2*h - 4);
	if (n < w)            p.x = n,     p.y = 0;
	else if (n < 2*w)     p.x = n - w, p.y = h - 1;
	else if (n < 2*w + h) p.x = 0,     p.y = n - 2*w;
	else                  p.x = w - 1, p.y = n - (2*w + h);
}
void Unit::destroy() {
	destroyed = true;
}
bool Unit::voyage(const W::position &_dest) {
  	arrived = false;
	dest = _dest;
	bool success = navmap->getRoute(pos.x, pos.y, dest.x, dest.y, &route);
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
//	W::Event ev;
//	ev.setType(W::EventType::INTERRUPT_UNITPICKUP);
//	ev._payload = this;
//	eh->dispatchEvent(&ev);
	
	if (skBehaviour) skBehaviour->unitWasPickedUp();
	
	using namespace W::EventType;
	W::Messenger::unsubscribeFromEventType(LEVEL_LEFTMOUSEDOWN,  this);
	W::Messenger::unsubscribeFromEventType(LEVEL_LEFTMOUSEUP,    this);
	W::Messenger::unsubscribeFromEventType(LEVEL_RIGHTMOUSEDOWN, this);
	W::Messenger::unsubscribeFromEventType(LEVEL_RIGHTMOUSEUP,   this);
	W::Messenger::unsubscribeFromEventType(LEVEL_MOUSEMOVE,      this);
	
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
	bool diagonal = pos.x != prev_pos.x && pos.y != prev_pos.y;
	
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
				prev_pos = pos;
				pos.x = c, pos.y = d;
				pos.a = prev_pos.x == pos.x ? 0 : prev_pos.x < pos.x ? -1 : 1;
				pos.b = prev_pos.y == pos.y ? 0 : prev_pos.y < pos.y ? -1 : 1;
				route.erase(route.begin());
			}
			else
				return navmap->getRoute(pos.x, pos.y, dest.x, dest.y, &route);
		}
	}
	else {
		// Decrement a & b toward current loc
		if (pos.x > prev_pos.x)      a_diff = step;
		else if (pos.x < prev_pos.x) a_diff = -step;
		if (pos.y > prev_pos.y)      b_diff = step;
		else if (pos.y < prev_pos.y) b_diff = -step;
		if (diagonal) a_diff *= 0.71, b_diff *= 0.71;	// For diagonal traveling, normalise the motion vector by dividing by √2
		pos.a += a_diff, pos.b += b_diff;
	}
	return true;
}
inline bool Unit::atDest() {
	return dest.x == pos.x && dest.y == pos.y && pos.a == 0 && pos.b == 0;
}
inline bool Unit::inHinterland() {
	return (
		pos.x < HINTERLAND_WIDTH || pos.x >= navmap->w-HINTERLAND_WIDTH ||
		pos.y < HINTERLAND_WIDTH || pos.y >= navmap->h-HINTERLAND_WIDTH
	);
}

std::vector<std::string>* Unit::getCompatibleBehaviours() {
	return &uInfo->compatibleBehaviours;
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
		W::log << "In units.lua, could not get defaults. Error: " << exc.what() << std::endl;
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
unitInfo * Unit::infoForType(const char *_type) {
	std::map<std::string, unitInfo>::iterator it = unitTypes.find(_type);
	if (it == unitTypes.end()) {
		char s[200];
		sprintf(s, "hireCostForType called with nonexistent type \"%s\"\n", _type);
		throw W::Exception(s);
	}
	return &it->second;
}

void Unit::printDebugInfo() {
	printf("\nUnit %p - x,y:%d,%d a,b:%.1f,%.1f dest:%d,%d mode:%s\n\n",
		this, pos.x, pos.y, pos.a, pos.b, dest.x, dest.y,
		mode == WAITING ? "WAITING" :
		mode == VOYAGING ? "VOYAGING" :
		mode == ANIMATING ? "ANIMATING" :
		mode == IDLE ? "IDLE" : "UNKNOWN"
	);
}
