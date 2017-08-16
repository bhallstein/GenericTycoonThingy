#include "Furniture.hpp"
#include "../W.hpp"
#include "LuaHelper.hpp"
#include "Building.hpp"
#include "NavMap.hpp"

std::map<std::string, struct furnitureInfo> Furniture::furnitureTypes;
std::string Furniture::defaultColour;
std::string Furniture::defaultHoverColour;

Furniture::Furniture(ResponderMap *_rm, NavMap *_navmap, const char *_type, Building *_context) :
	MappedObj(_rm, true), navmap(_navmap), contextBuilding(_context)
{
	// Set properties for this Furniture type
	groundplan = furnitureTypes[_type].groundplan;
	f_colour      = &furnitureTypes[_type].col;
	f_hoverColour = &furnitureTypes[_type].hoverCol;
}
Furniture::~Furniture()
{
	std::cout << "furniture destruct" << std::endl;
	navmap->removeImpassableObject(this);
}

void Furniture::receiveEvent(Event *ev) {
	if (ev->type == Event::MOUSEMOVE) hover = true;
	else if (ev->type == Event::LEFTCLICK)
		std::cout << "click!" << std::endl;
}

bool isEntirelyInsideBuilding(std::vector<intcoord> *gp, int _x, int _y, Building *b) {
	std::vector<intcoord> *b_gp = &b->groundplan;
	int bx = b->x, by = b->y;
	for (int i=0, n = gp->size(); i < n; i++) {
		int x = _x + gp->at(i).x, y = _y + gp->at(i).y;
		bool in_building_gp = false;
		for (int j=0, n = b_gp->size(); j < n; j++)
			if (bx + b_gp->at(j).x == x && by + b_gp->at(j).y == y) {
				in_building_gp = true;
				break;
			}
		if (!in_building_gp) return false;
	}
	return true;
}

bool Furniture::canPlace(int _x, int _y) {
	if (contextBuilding)
		return navmap->isPassableUnder(&groundplan, _x, _y) && isEntirelyInsideBuilding(&groundplan, _x, _y, contextBuilding);
	return navmap->isPassableUnder(&groundplan, _x, _y);
}

void Furniture::finalizePlacement() {
	navmap->addImpassableObject(this);
}

const char * Furniture::col() {
	if (hover) { hover = false; return f_hoverColour->c_str(); }
	return f_colour->c_str();
}

bool Furniture::initialize(W *_W) {
	W::log("furniture::initialize() called...");
	
	LuaHelper mrLua(_W);
	
	std::string path = _W->resourcesPath;
	path.append("furniture.lua");
	if (!mrLua.loadFile(path.c_str())) {
		W::log("Could not read furniture.lua");
		return false;
	}
	lua_State *L = mrLua.LuaInstance;
	
	try {
		// Set furniture defaults
		Furniture::defaultColour             = mrLua.getvalue<const char *>("defaultColour");
		Furniture::defaultHoverColour        = mrLua.getvalue<const char *>("defaultHoverColour");
	} catch (MsgException &exc) {
		std::string s = "In furniture.lua, could not get defaults. Error: "; s.append(exc.msg);
		W::log(s.c_str());
	}
	
	// Construct furniture::furnitureTypes map
	if (!mrLua.pushtable("furnitureTypes")) {
		W::log("In furniture.lua, could not push the furnitureTypes table onto the stack");
		return false;
	}
	lua_pushnil(L);									// S: -1 nil; -2 table
	while (lua_next(L, -2) != 0) {					// S: -1 subtable; -2 key; -3 table
		if (!lua_istable(L, -1)) {			// If not a subtable, skip
			lua_pop(L, 1);
			continue;
		}
		const char *f_type = lua_tostring(L, -2);
		struct furnitureInfo *fInfo = &Furniture::furnitureTypes[f_type];
		
		lua_getfield(L, -1, "colour");				// S: -1 colour; -2 subtable; -3 key; -4 table
		fInfo->col = lua_isstring(L, -1) ? lua_tostring(L, -1) : Furniture::defaultColour;
		lua_pop(L, 1);
		
		lua_getfield(L, -1, "hoverColour");			// S: -1 colour; -2 subtable; -3 key; -4 table
		fInfo->hoverCol = lua_isstring(L, -1) ? lua_tostring(L, -1) : Furniture::defaultHoverColour;
		lua_pop(L, 1);
		
		// Ground plans
		if (!mrLua.pushSubtable("groundplan")) {
			char s[100]; sprintf(s, "In furniture.lua, could not find groundplan for '%s' type", f_type);
			W::log(s);
			return false;
		}											// S: -1 groundplan; -2 subtable; -3 key; -4 table
		lua_pushnil(L);								// S: -1 nil; -2 groundplan; ...
		while (lua_next(L, -2)) {					// S: -1 value; -2 key; -3 groundplan
			intcoord c = { mrLua.getfield<int>(1), mrLua.getfield<int>(2) };
			fInfo->groundplan.push_back(c);
			lua_pop(L, 1);
		}											// S: -1 groundplan; -2 subtable; -3 key; -4 table
		
		lua_pop(L, 2);								// S: -1 key; -2 table
	}
	
	W::log("...initialization succeeded.");
	return true;
}
