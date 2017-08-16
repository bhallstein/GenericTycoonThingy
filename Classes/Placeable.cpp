#include "Placeable.hpp"
#include "NavMap.hpp"
#include "ResponderMap.hpp"
#include "../W.hpp"
#include "LuaHelper.hpp"

std::map<std::string, struct placeableInfo> Placeable::placeableTypes;
std::string Placeable::defaultColour;
std::string Placeable::defaultHoverColour;
std::string Placeable::defaultColourWhilePlacing;

Placeable::Placeable(NavMap *_navmap, ResponderMap *_levelRM, const char *_type) :
	MappedObj(-100, -100), navmap(_navmap), levelResponderMap(_levelRM), type(_type), clicked(false), destroyed(false), mode(PLACEMENT)
{
	intcoord p[] = {
		{0,0}, {1,0},
		{0,1}, {1,1},	{-1,-1}
	};
	setGroundPlan(p);
	
	// Set properties for this Placeable type
	p_colour             = placeableTypes[_type].col;
	p_hoverColour        = placeableTypes[_type].hoverCol;
	p_colourWhilePlacing = placeableTypes[_type].colWhilePlacing;
}
Placeable::~Placeable()
{
	std::cout << "placeable destruct" << std::endl;
}

void Placeable::receiveEvent(Event *ev) {
	if (mode == PLACEMENT) {
		if (ev->type == Event::MOUSEMOVE) {
			x = ev->x, y = ev->y;
		}
		else if (ev->type == Event::LEFTCLICK) {
			for (int i=0, n = ground_plan.size(); i < n; i++) {
				intcoord c = ground_plan[i];
				if (!navmap->isPassableAt(c.x + x, c.y + y))
					return;
			}
			mode = PLACED;
			levelResponderMap->relinquishPrivilegedEventResponderStatus(this);
			levelResponderMap->addMappedObj(this);
			navmap->addImpassableObject(this);
		}
		else if (ev->type == Event::RIGHTCLICK) {
			levelResponderMap->relinquishPrivilegedEventResponderStatus(this);
			destroyed = true;
		}
	}
	else if (mode == PLACED) {
		if (ev->type == Event::LEFTCLICK)      clicked = !clicked;
		else if (ev->type == Event::MOUSEMOVE) hover = true;
	}
}

const char * Placeable::col() {
	if (mode == PLACEMENT) return p_colourWhilePlacing.c_str();
	if (hover) { hover = false; return p_hoverColour.c_str(); }
	return p_colour.c_str();
}

bool Placeable::initialize(W *_W) {
	W::log("Placeable::initialize() called...");
	
	LuaHelper mrLua(_W);
	
	std::string path = _W->resourcesPath;
	path.append("placeables.lua");
	if (!mrLua.loadFile(path.c_str())) {
		W::log("Could not read placeables.lua");
		return false;
	}
	lua_State *L = mrLua.LuaInstance;
	
	// Set Placeable::defaultColour
	lua_getglobal(L, "defaultColour");				// S: -1 value
	if (!lua_isstring(L, -1)) {
		W::log("In placeables.lua, could not find defaultColour");
		return false;
	}
	Placeable::defaultColour = lua_tostring(L, -1);
	lua_pop(L, 1);									// S: ~
	
	// Set Placeable::defaultHoverColour
	lua_getglobal(L, "defaultHoverColour");			// S: -1 value
	if (!lua_isstring(L, -1)) {
		W::log("In placeables.lua, could not find defaultHoverColour");
		return false;
	}
	Placeable::defaultHoverColour = lua_tostring(L, -1);
	lua_pop(L, 1);									// S: ~
	
	// Set Placeable::defaultColourWhilePlacing
	lua_getglobal(L, "defaultColourWhilePlacing");			// S: -1 value
	if (!lua_isstring(L, -1)) {
		W::log("In placeables.lua, could not find defaultColourWhilePlacing");
		return false;
	}
	Placeable::defaultColourWhilePlacing = lua_tostring(L, -1);
	lua_pop(L, 1);									// S: ~
	
	// Construct Placeable::placeableTypes map
	if (!mrLua.pushtable("placeableTypes")) {
		W::log("In placeables.lua, could not push the placeableTypes table onto the stack");
		return false;
	}
	lua_pushnil(L);									// S: -1 nil; -2 table
	while (lua_next(L, -2) != 0) {					// S: -1 subtable; -2 key; -3 table
		if (!lua_istable(L, -1)) {			// If not a subtable, skip
			lua_pop(L, 1);
			continue;
		}
		const char *p_type = lua_tostring(L, -2);
		struct placeableInfo *pInfo = &Placeable::placeableTypes[p_type];
		
		lua_getfield(L, -1, "colour");				// S: -1 colour; -2 subtable; -3 key; -4 table
		pInfo->col = lua_isstring(L, -1) ? lua_tostring(L, -1) : Placeable::defaultColour;
		lua_pop(L, 1);
		
		lua_getfield(L, -1, "hoverColour");			// S: -1 colour; -2 subtable; -3 key; -4 table
		pInfo->hoverCol = lua_isstring(L, -1) ? lua_tostring(L, -1) : Placeable::defaultHoverColour;
		lua_pop(L, 1);
		
		lua_getfield(L, -1, "colourWhilePlacing");	// S: -1 colour; -2 subtable; -3 key; -4 table
		pInfo->colWhilePlacing = lua_isstring(L, -1) ? lua_tostring(L, -1) : Placeable::defaultColourWhilePlacing;
		
		lua_pop(L, 2);								// S: -1 key; -2 table
	}
	
	W::log("...initialization succeeded.");
	return true;
}
