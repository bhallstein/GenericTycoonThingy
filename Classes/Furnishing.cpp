#include "Furnishing.hpp"
#include "LuaHelper.hpp"
#include "Building.hpp"
#include "Unit.hpp"
#include "Level.hpp"
#include "MrPaths.hpp"
#include "W.h"

std::map<std::string, struct furnishingInfo> Furnishing::furnishingTypes;
W::Colour Furnishing::defaultColour;
W::Colour Furnishing::defaultHoverColour;
bool Furnishing::initialized = false;

Furnishing::Furnishing(W::EventHandler *_eh, W::NavMap *_navmap, const char *_type, Level *_level, Building *_context, bool _placeableMode) :
	PlaceableManager(_eh, _placeableMode), navmap(_navmap), type(_type), level(_level), contextBuilding(_context),
	animationFinished(false), purchased(false)
{
	// Set properties for this Furnishing type
	fInfo = &furnishingTypes[type];
	plan = fInfo->plan;
}
Furnishing::~Furnishing()
{
	std::cout << "furnishing destruct" << std::endl;
	navmap->makePassable(this);
	if (contextBuilding)
		contextBuilding->removeFurnishing(this);
}

void Furnishing::receiveEvent(W::Event *ev) {
	using namespace W::EventType;
	
	if (ev->type == LEVEL_MOUSEMOVE) hover = true;
	else if (ev->type == LEVEL_LEFTMOUSEDOWN)
		std::cout << "click!" << std::endl;
}

void Furnishing::update() {
//	if (animating) {
//		if (++animFrames == animationDuration)
//			animating = false, animationFinished = true;
//	}
}

bool Furnishing::canPlace(int _x, int _y) {
	int prev_x = pos.x, prev_y = pos.y;
	pos.x = _x, pos.y = _y;
	bool can_place = contextBuilding
		? navmap->isPassableUnder(this) && contextBuilding->objIsEntirelyInsideBuilding(this)
		: navmap->isPassableUnder(this);
	pos.x = prev_x, pos.y = prev_y;
	return can_place;
}

void Furnishing::finalizePlacement() {
	std::cout << "Furnishing: finalizePlacement" << std::endl;
	navmap->makeImpassable(this);
	if (contextBuilding) {
		printf("Adding furnishing %p to building %p\n", this, contextBuilding);
		contextBuilding->addFurnishing(this);
	}
	if (!purchased) {
		level->chargePlayer(Furnishing::costForType(type.c_str()));
		purchased = true;
	}
	using namespace W::EventType;
	eh->subscribeToEventType(LEVEL_LEFTMOUSEDOWN,  W::Callback(&Furnishing::receiveEvent, this));
	eh->subscribeToEventType(LEVEL_LEFTMOUSEUP,    W::Callback(&Furnishing::receiveEvent, this));
	eh->subscribeToEventType(LEVEL_RIGHTMOUSEDOWN, W::Callback(&Furnishing::receiveEvent, this));
	eh->subscribeToEventType(LEVEL_RIGHTMOUSEUP,   W::Callback(&Furnishing::receiveEvent, this));
	eh->subscribeToEventType(LEVEL_MOUSEMOVE,      W::Callback(&Furnishing::receiveEvent, this));
}

W::Colour& Furnishing::col() {
	if (hover) { hover = false; return fInfo->hoverCol; }
	return fInfo->col;
}

void Furnishing::getInteractionPoint(const char *uType, W::position &p) {
	std::map<std::string, W::position>::iterator it = fInfo->interactionPoints.find(uType);
	if (it == fInfo->interactionPoints.end()) {
		char s[200];
		sprintf(s, "No interaction point for unit type %s in furnishing type %s", uType, type.c_str());
		throw W::Exception(s);
	}
	else {
		p.x = it->second.x + pos.x;
		p.y = it->second.y + pos.y;
	}
}
bool Furnishing::requiresStaff(const char *uType) {
	return fInfo->interactionPoints.count(uType);
}
void Furnishing::runAnimation(int duration) {
//	animationDuration = duration;
//	animFrames = 0;
//	animating = true;
}

std::vector<std::string>* Furnishing::getCompatibleBehaviours() {
	return &fInfo->compatibleBehaviours;
}

bool Furnishing::initialize() {
	if (Furnishing::initialized) return true;
	W::log << "  Furnishing::initialize() called..." << std::endl;
	
	LuaHelper mrLua;
	
	std::string path = MrPaths::luaPath;
	path.append("furnishings.lua");
	if (!mrLua.loadFile(path.c_str())) {
		W::log << "Could not read furnishings.lua" << std::endl;
		return false;
	}
	lua_State *L = mrLua.LuaInstance;
	
	try {
		// Set defaults
		Furnishing::defaultColour      = strToColour(mrLua.getvalue<const char *>("defaultColour"));
		Furnishing::defaultHoverColour = strToColour(mrLua.getvalue<const char *>("defaultHoverColour"));
	} catch (W::Exception &exc) {
		W::log << "In furnishings.lua, could not get defaults. Error: " << exc.what() << std::endl;
	}
	
	// Construct furnishingTypes map
	if (!mrLua.pushtable("furnishingTypes")) {
		W::log << "In furnishings.lua, could not push the furnishingTypes table onto the stack" << std::endl;
		return false;
	}
	lua_pushnil(L);									// S: nil, furnishingTypes
	while (lua_next(L, -2)) {						// S: furnishing table, key, furnishingTypes
		if (!lua_istable(L, -1)) {
			lua_pop(L, 1);
			continue;
		}
		const char *fType = lua_tostring(L, -2);
		struct furnishingInfo *fInfo = &Furnishing::furnishingTypes[fType];
		
		W::log << "    " << fType << std::endl;
		
		lua_getfield(L, -1, "colour");				// S: colour, furnishing table, key, furnishingTypes
		if (lua_isstring(L, -1))
			fInfo->col = strToColour(lua_tostring(L, -1));
		else
			fInfo->col = Furnishing::defaultColour;
		lua_pop(L, 1);
		
		lua_getfield(L, -1, "hoverColour");			// S: hoverColour, furnishing table, key, furnishingTypes
		if (lua_isstring(L, -1))
			fInfo->hoverCol = strToColour(lua_tostring(L, -1));
		else
			fInfo->hoverCol = Furnishing::defaultHoverColour;
		lua_pop(L, 1);

		lua_getfield(L, -1, "cost");				// S: cost, furnishing table, key, furnishingTypes
		fInfo->cost = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : 0;
		lua_pop(L, 1);								// S: furnishing table, key, furnishingTypes

		// Ground plan
		std::vector<W::rect> *fPlan = &fInfo->plan;
		if (!mrLua.pushSubtable("plan"))				// S: plan, furnishing table, key, furnishingTypes
			throw W::Exception("Could not find plan table.");
		lua_pushnil(L);									// S: nil, plan, ...
		while (lua_next(L, -2)) {						// S: rect, key, plan, ...
			if (!lua_istable(L, -1)) {
				lua_pop(L, 1);
				continue;
			}
			W::rect r = {
				W::position(mrLua.getSubfield<int>("pos", 1), mrLua.getSubfield<int>("pos", 2)),
				W::size(mrLua.getSubfield<int>("sz", 1), mrLua.getSubfield<int>("sz", 2))
			};
			fPlan->push_back(r);
			lua_pop(L, 1);								// S: key, plan, ...
		}												// S: plan, furnishing table, key, furnishingTypes
		lua_pop(L, 1);									// S: furnishing table, key, furnishingTypes
		W::log << "      rects:";
		for (std::vector<W::rect>::iterator it = fPlan->begin() ; it < fPlan->end(); it++)
			W::log << " " << it->pos.x << "," << it->pos.y << "->" << it->pos.x + it->sz.width << "," << it->pos.y + it->sz.height;
		W::log << std::endl;
		
		// Compatible Behaviours
		if (mrLua.pushSubtable("compatibleBehaviours")) {
			lua_pushnil(L);							// S: nil, compatibleBehaviours, furnishing table, ...
			while (lua_next(L, -2)) {				// S: compatible behaviour, key, compatibleBehaviours, ...
				if (!lua_isstring(L, -1)) {
					lua_pop(L, 1);
					continue;
				}
				std::string iType = lua_tostring(L, -1);
				fInfo->compatibleBehaviours.push_back(iType);
				lua_pop(L, 1);						// S: key, compatibleBehaviours, ...
			}										// S: compatibleBehaviours, furnishing table, key, furnishingTypes
			lua_pop(L, 1);							// S: furnishingTable, key, table
		}											// S: compatibleBehaviours, furnishing table, key, furnishingTypes
		W::log << "      compatibleBehaviours:";
		if (fInfo->compatibleBehaviours.size())
			for (std::vector<std::string>::iterator it = fInfo->compatibleBehaviours.begin(); it < fInfo->compatibleBehaviours.end(); it++)
				W::log << " " << *it;
		else
			W::log << " none";
		W::log << std::endl;
		
		// Interaction Points
		if (mrLua.pushSubtable("interactionPoints")) {
			lua_pushnil(L);								// S: nil, interactionPoints, furnishing table, key, furnishingTypes
			while (lua_next(L, -2)) {					// S: interaction point, unittype, interactionPoints, ...
				if (!lua_isstring(L, -2) || !lua_istable(L, -1)) {
					lua_pop(L, 1);
					continue;
				}
				try {
					std::string uType = lua_tostring(L, -2);
					W::position p;
					p.x = mrLua.getfield<int>(1);
					p.y = mrLua.getfield<int>(2);
					
					fInfo->interactionPoints[uType] = p;
				} catch(W::Exception &exc) {
					W::log << "Invalid coordinates for compatibleBehaviour for '" << fType << "' type" << std::endl;
					return false;
				}
				lua_pop(L, 1);					// S: unittype, interactionPoints, furnishing table, key, furnishingTypes
			}									// S: interactionPoints, furnishing table, key, furnishingTypes
			lua_pop(L, 1);						// S: furnishing table, key, furnishingTypes
		}
		W::log << "      interactionPoints:";
		if (fInfo->interactionPoints.size())
			for (std::map<std::string, W::position>::iterator it = fInfo->interactionPoints.begin(); it != fInfo->interactionPoints.end(); it++)
				W::log << " " << it->first << "=>{" << it->second.x << "," << it->second.y << "}";
		else
			W::log << " none";
		W::log << std::endl;
		
		lua_pop(L, 1);							// S: keyfurnishingTypes
	}

	Furnishing::initialized = true;
	return true;
}
int Furnishing::costForType(const char *_type) {
	return furnishingTypes[_type].cost;
}


Door::Door(W::EventHandler *_eh, W::NavMap *_nm, const char *_type, Level *_level, bool _placeableMode) :
	Furnishing(_eh, _nm, _type, _level, NULL, _placeableMode)
{
	
}
Door::~Door()
{
	// bai doory
}

bool Door::canPlace(int _x, int _y) {
	return true;
}
void Door::finalizePlacement() {
	// Recreate door connections
	// Get door coordinates
	std::vector<W::position> dcoords;
	W::rect drect = plan[0];
	for (int i = pos.x + drect.pos.x; i < pos.x + drect.pos.x + drect.sz.width; i++)
		for (int j = pos.y + drect.pos.y; j < pos.y + drect.pos.y + drect.sz.height; j++)
			dcoords.push_back(W::position(i, j));
	if (dcoords.size() != 2)
		W::log << "Door found with " << dcoords.size() << " coordinates in plan!" << std::endl;
	else
		navmap->createConnection(dcoords[0], dcoords[1]);
	
	return;
}
