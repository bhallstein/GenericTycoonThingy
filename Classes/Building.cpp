#include "Building.hpp"
#include "LuaHelper.hpp"
#include "../W.hpp"

// Initialize static members
std::map<std::string, struct buildingInfo> Building::buildingTypes;
colour Building::defaultColour;
colour Building::defaultHoverColour;

Building::Building(int _x, int _y, const char *_type) : MappedObj(_x, _y), type(_type), clicked(false), destroyed(false), time_hovered(0)
{
	intcoord p[] = {
		{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},
		{0,1}, {1,1}, {2,1}, {3,1}, {4,1}, {5,1},
		{0,2}, {1,2}, {2,2}, {3,2}, {4,2}, {5,2},
		{0,3}, {1,3}, {2,3}, {3,3}, {4,3}, {5,3},
		              {2,4}, {3,4},					{-1,-1}
	};
	setGroundPlan(p);
	door d1 = {2,4, DOWNWARD};
	door d2 = {3,4, DOWNWARD};
	door d3 = {0,1, LEFTWARD};		// Side door! Why not.
	doors.push_back(d1);
	doors.push_back(d2);
	doors.push_back(d3);
	
	// Get properties for this building type.
	b_colour = Building::buildingTypes[type].col;
	b_hoverColour = Building::buildingTypes[type].hovercol;
}
Building::~Building()
{
	std::cout << "building destruct" << std::endl;
}

bool Building::initialize(W *_W) {
	_W->log("Building::initialize() called...");

	LuaHelper mrLua(_W);
	
	std::string path = _W->resourcesPath;
	path.append("buildings.lua");
	if (!mrLua.loadFile(path.c_str())) {
		_W->log("Building: could not read buildings.lua");
		return false;
	}
	lua_State *L = mrLua.LuaInstance;
	
	// Set Building::defaultColour
	lua_getglobal(L, "defaultColour");
	if (!lua_isstring(L, -1)) {
		_W->log("In buildings.lua, could not find defaultColour");
		return false;
	}
	Building::defaultColour = colour(lua_tostring(L, -1));
	lua_pop(L, 1);
	
	// Set Building::defaultHoverColour
	lua_getglobal(L, "defaultHoverColour");
	if (!lua_isstring(L, -1)) {
		_W->log("In buildings.lua, could not find defaultHoverColour");
		return false;
	}
	Building::defaultHoverColour = colour(lua_tostring(L, -1));
	lua_pop(L, 1);
	
	// Construct Building::buildingTypes map
	if (!mrLua.pushtable("buildingTypes")) {
		_W->log("In buildings.lua, could not push the buildingTypes table onto the stack");
		return false;
	}
	lua_pushnil(L);									// Stack: -1 nil; -2 table
	while (lua_next(L, -2) != 0) {					// Stack: -1 subtable; -2 key; -3 table
		if (lua_type(L, -1) != LUA_TTABLE) {		// If not a subtable, skip
			lua_pop(L, 1);
			continue;
		}
		const char *b_type = lua_tostring(L, -2);
		struct buildingInfo *bInfo = &Building::buildingTypes[b_type];
		
		lua_getfield(L, -1, "colour");				// Stack: -1 colour; -2 subtable; -3 key; -4 table
		if (!lua_isstring(L, -1)) bInfo->col = Building::defaultColour;
		else bInfo->col = colour(lua_tostring(L, -1));
		lua_pop(L, 1);
		
		lua_getfield(L, -1, "hoverColour");			// Stack: -1 colour; -2 subtable; -3 key; -4 table
		if (!lua_isstring(L, -1)) bInfo->hovercol = Building::defaultHoverColour;
		else bInfo->hovercol = colour(lua_tostring(L, -1));
		
		lua_pop(L, 2);								// Stack: -1 key; -2 table
	}
	
	return true;
}

void Building::receiveEvent(Event *ev) {
	if (ev->type == Event::LEFTCLICK)
		clicked = !clicked;
	else if (ev->key == Event::K_L)
		std::cout << "Building " << this << " detected the L key!" << std::endl;
	else if (ev->type == Event::MOUSEMOVE)
		if (++time_hovered == 60) {			// Eventually want an actual time, not a frame count
			
		}
}

colour Building::col() {
	if (hover) { hover = false; return b_hoverColour; }
	return b_colour;
}
