#include "Building.hpp"
#include "LuaHelper.hpp"
#include "../W.hpp"
#include "Level.hpp"
#include "NavMap.hpp"

// Initialize static members
std::map<std::string, struct buildingInfo> Building::buildingTypes;
std::string Building::defaultColour;
std::string Building::defaultHoverColour;

Building::Building(ResponderMap *_rm, NavMap *_nm, const char *_type, std::vector<intcoord> *_groundplan, std::vector<door> *_doors, Level *_level) :
	MappedObj(_rm, false), navmap(_nm), type(_type), level(_level), clicked(false), time_hovered(0)
{	
	setGroundPlan(_groundplan);
	doors = *_doors;
	// Get properties for this building type
	b_colour           = &Building::buildingTypes[type].col;
	b_hoverColour      = &Building::buildingTypes[type].hoverCol;
	b_allowedFurniture = &Building::buildingTypes[type].allowedFurniture;
}
Building::~Building()
{
	std::cout << "building destruct" << std::endl;
	navmap->removeImpassableObject(this);
}

bool Building::canPlace(int _x, int _y) {
	return navmap->isPassableUnder(&groundplan, _x, _y);
}
void Building::finalizePlacement() {
	navmap->addBuilding(this);
}

void Building::receiveEvent(Event *ev) {
	if (ev->type == Event::LEFTCLICK) level->openFurniturePurchasingView(this);
	else if (ev->key == Event::K_L)	  std::cout << "Building " << this << " detected the L key!" << std::endl;
}

const char * Building::col() {
	if (hover) { hover = false; return b_hoverColour->c_str(); }
	return b_colour->c_str();
}

void Building::getEntryPoint(int *_x, int *_y) {
	door d = doors[W::randUpTo(doors.size())];
	*_x = x + d.coord.x;
	*_y = y + d.coord.y;
}
void Building::getQueuePoint(int *_x, int *_y) {
	*_x = x;
	*_y = y;
}

bool Building::initialize(W *_W) {
	W::log("Building::initialize() called...");
	
	LuaHelper mrLua(_W);
	
	std::string path = _W->resourcesPath;
	path.append("buildings.lua");
	if (!mrLua.loadFile(path.c_str())) {
		W::log("Could not read buildings.lua");
		return false;
	}
	lua_State *L = mrLua.LuaInstance;
	
	try {
		// Set Building defaults
		Building::defaultColour      = mrLua.getvalue<const char *>("defaultColour");
		Building::defaultHoverColour = mrLua.getvalue<const char *>("defaultHoverColour");
	} catch (MsgException &exc) {
		std::string s = "In buildings.lua, could not get defaults. Error: "; s.append(exc.msg);
		W::log(s.c_str());
	}
	
	// Construct Building::buildingTypes map
	if (!mrLua.pushtable("buildingTypes")) {
		W::log("In buildings.lua, could not push the buildingTypes table onto the stack");
		return false;
	}
	lua_pushnil(L);								// S: -1 nil; -2 buildingTypes
	while (lua_next(L, -2)) {					// S: -1 buildingtype; -2 key; -3 buildingTypes
		if (!lua_istable(L, -1)) {		// If not a subtable, skip
			lua_pop(L, 1);
			continue;
		}
		const char *b_type = lua_tostring(L, -2);
		struct buildingInfo *bInfo = &Building::buildingTypes[b_type];
		
		lua_getfield(L, -1, "colour");			// S: -1 colour; -2 buildingtype; -3 key; -4 buildingTypes
		bInfo->col = lua_isstring(L, -1) ? lua_tostring(L, -1) : Building::defaultColour;
		lua_pop(L, 1);
		
		lua_getfield(L, -1, "hoverColour");		// S: -1 colour; -2 buildingtype; -3 key; -4 buildingTypes
		bInfo->hoverCol = lua_isstring(L, -1) ? lua_tostring(L, -1) : Building::defaultHoverColour;
		lua_pop(L, 1);							// S: -1 buildingtype; -2 key; -3 buildingTypes

		if (mrLua.pushSubtable("allowedFurniture")) {
			lua_pushnil(L);						// S: -1 nil; -2 allowedFurniture; -3 buildingtype; ...
			int n = 0;
			std::string s = b_type; s += ": allowedFurniture: ";
			while (lua_next(L, -2)) {			// S: -1 value; -2 key; -3 allowedFurniture
				if (!lua_isstring(L, -1)) {
					lua_pop(L, 1);
					continue;
				}
				bInfo->allowedFurniture.push_back(lua_tostring(L, -1));
				s += bInfo->allowedFurniture.back(); s += ", ";
				n++;
				lua_pop(L, 1);					// S: -1 key; -2 allowedFurniture; -3 buildingtype; ...
			}									// S: -1 allowedFurniture; -2 buildingtype; ...
			lua_pop(L, 1);						// S: -1 buildingtype; -2 key; -3 buildingTypes
			if (n > 0) {
				s.erase(s.size() - 2);
				W::log(s.c_str());
			}
		}
		
		lua_pop(L, 1);							// S: -1 key; -2 buildingTypes
	}
	
	_W->log("...initialization succeeded.");
	return true;
}
