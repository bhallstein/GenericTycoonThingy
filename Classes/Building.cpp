#include "Building.hpp"
#include "LuaHelper.hpp"
#include "../W.hpp"
#include "Level.hpp"
#include "NavMap.hpp"
#include "Furnishing.hpp"

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
	b_colour             = &Building::buildingTypes[type].col;
	b_hoverColour        = &Building::buildingTypes[type].hoverCol;
	b_allowedFurnishings = &Building::buildingTypes[type].allowedFurnishings;
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
	if (ev->type == Event::LEFTCLICK) level->openFurnishingPurchasingView(this);
	else if (ev->key == Event::K_L)	  std::cout << "Building " << this << " detected the L key!" << std::endl;
}

void Building::update() {
	int indices[furnishings.size()], nFound ;
	// Queue dispatch
	for (std::vector<Unit*>::iterator it = Q.begin(); it < Q.end(); ) {
		Unit *u = *it, *s;
		nFound = 0;
		// Get random Furnishing f ready for behaviour
		for (int i=0, n = furnishings.size(); i < n; i++) {
			Furnishing *f = furnishings[i];
			if (f->readyForInteraction("havehaircut"))
				if (staffBindings.find(f) != staffBindings.end())
					s = staffBindings[f], indices[nFound++] = i;
		}
		if (nFound == 0)
			it++;
		else {
			Furnishing *f = furnishings[indices[W::randUpTo(nFound)]];
			level->createBehaviour("havehaircut")->init(level, u, s, f);
			it = Q.erase(it);
		}
	}
}

const char * Building::col() {
	if (hover) { hover = false; return b_hoverColour->c_str(); }
	return b_colour->c_str();
}

void Building::getEntryPoint(int *_x, int *_y) {
	door d = doors[W::randUpTo(doors.size())];
	*_x = x + d.coord.x;
	*_y = y + d.coord.y;
	if (d.orientation == Direction::DOWNWARD)       *_y += 1;
	else if (d.orientation == Direction::UPWARD)    *_y -= 1;
	else if (d.orientation == Direction::RIGHTWARD) *_x += 1;
	else if (d.orientation == Direction::LEFTWARD)  *_x -= 1;
}
void Building::getQueuePoint(int *_x, int *_y) {
	*_x = x;
	*_y = y;
}
void Building::addToQueue(Unit *u) {
	Q.push_back(u);
}
void Building::addFurnishing(Furnishing *f) {
	furnishings.push_back(f);
}
void Building::removeFurnishing(Furnishing *f) {
	for (std::vector<Furnishing *>::iterator it = furnishings.begin(); it != furnishings.end(); it++)
		if (*it == f) {
			furnishings.erase(it);
			return;
		}
}
void Building::addStaff(Unit *s) {
	std::cout << "adding staff to building... ";
	staff.push_back(s);
	// Add to an unstaffed furnishing
	for (int i=0, n = furnishings.size(); i < n; i++) {
		Furnishing *f = furnishings[i];
		if (f->requiresStaff(s->type.c_str()) && staffBindings.find(f) == staffBindings.end())
			staffBindings[f] = s;
	}
}

bool Building::initialize(W *_W) {
	W::log("  Building::initialize() called...");
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
		const char *bType = lua_tostring(L, -2);
		struct buildingInfo *bInfo = &Building::buildingTypes[bType];
		
		lua_getfield(L, -1, "colour");			// S: -1 colour; -2 buildingtype; -3 key; -4 buildingTypes
		bInfo->col = lua_isstring(L, -1) ? lua_tostring(L, -1) : Building::defaultColour;
		lua_pop(L, 1);
		
		lua_getfield(L, -1, "hoverColour");		// S: -1 colour; -2 buildingtype; -3 key; -4 buildingTypes
		bInfo->hoverCol = lua_isstring(L, -1) ? lua_tostring(L, -1) : Building::defaultHoverColour;
		lua_pop(L, 1);							// S: -1 buildingtype; -2 key; -3 buildingTypes

		if (!mrLua.pushSubtable("allowedFurnishing")) {
			char s[100]; sprintf(s, "In buildings.lua, couldn't find allowedFurnishing for '%s' type", bType);
			W::log(s);
			return false;
		}
		std::string s = "    "; s += bType; s += ": allowedFurnishing: ";
		int n = 0;
		lua_pushnil(L);							// S: -1 nil; -2 allowedFurnishing; -3 buildingtype; ...
		while (lua_next(L, -2)) {				// S: -1 value; -2 key; -3 allowedFurnishing
			if (!lua_isstring(L, -1)) {
				lua_pop(L, 1);
				continue;
			}
			bInfo->allowedFurnishings.push_back(lua_tostring(L, -1));
			s += bInfo->allowedFurnishings.back(); s += ", ";
			n++;
			lua_pop(L, 1);						// S: -1 key; -2 allowedFurnishing; -3 buildingtype; ...
		}										// S: -1 allowedFurnishing; -2 buildingtype; ...
		lua_pop(L, 1);							// S: -1 buildingtype; -2 key; -3 buildingTypes
		if (n > 0) 
			s.erase(s.size() - 2);
		W::log(s.c_str());
		
		lua_pop(L, 1);							// S: -1 key; -2 buildingTypes
	}
	
	return true;
}
