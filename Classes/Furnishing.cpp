#include "Furnishing.hpp"
#include "../W.hpp"
#include "LuaHelper.hpp"
#include "Building.hpp"
#include "NavMap.hpp"
#include "Unit.hpp"

std::map<std::string, struct furnishingInfo> Furnishing::furnishingTypes;
std::string Furnishing::defaultColour;
std::string Furnishing::defaultHoverColour;
bool Furnishing::initialized = false;

Furnishing::Furnishing(ResponderMap *_rm, NavMap *_navmap, const char *_type, Building *_context) :
	MappedObj(_rm, true), navmap(_navmap), type(_type), contextBuilding(_context),
	animationFinished(false)
{
	// Set properties for this Furnishing type
	groundplan             = furnishingTypes[type].groundplan;
	f_colour               = &furnishingTypes[type].col;
	f_hoverColour          = &furnishingTypes[type].hoverCol;
	f_compatibleBehaviours = &furnishingTypes[type].compatibleBehaviours;
	f_interactionPoints    = &furnishingTypes[type].interactionPoints;
}
Furnishing::~Furnishing()
{
	std::cout << "furnishing destruct" << std::endl;
	navmap->removeImpassableObject(this);
	if (contextBuilding)
		contextBuilding->removeFurnishing(this);
}

void Furnishing::receiveEvent(Event *ev) {
	if (ev->type == Event::MOUSEMOVE) hover = true;
	else if (ev->type == Event::LEFTCLICK)
		std::cout << "click!" << std::endl;
}

void Furnishing::update() {
//	if (animating) {
//		if (++animFrames == animationDuration)
//			animating = false, animationFinished = true;
//	}
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

bool Furnishing::canPlace(int _x, int _y) {
	if (contextBuilding)
		return navmap->isPassableUnder(&groundplan, _x, _y) && isEntirelyInsideBuilding(&groundplan, _x, _y, contextBuilding);
	return navmap->isPassableUnder(&groundplan, _x, _y);
}

void Furnishing::finalizePlacement() {
	navmap->addImpassableObject(this);
	if (contextBuilding)
		contextBuilding->addFurnishing(this);
}

const char * Furnishing::col() {
	if (hover) { hover = false; return f_hoverColour->c_str(); }
	return f_colour->c_str();
}

void Furnishing::getInteractionPoint(const char *uType, int *_x, int *_y) {
	std::map<std::string, intcoord>::iterator it = f_interactionPoints->find(uType);
	if (it == f_interactionPoints->end())
		*_x = *_y = -1;
	else {
		*_x = it->second.x + x;
		*_y = it->second.y + y;
	}
}
bool Furnishing::requiresStaff(const char *uType) {
	return f_interactionPoints->count(uType);
}
void Furnishing::runAnimation(int duration) {
//	animationDuration = duration;
//	animFrames = 0;
//	animating = true;
}

std::vector<std::string>* Furnishing::getCompatibleBehaviours() {
	return f_compatibleBehaviours;
}

bool Furnishing::initialize(W *_W) {
	if (Furnishing::initialized) return true;
	
	W::log("  Furnishing::initialize() called...");
	LuaHelper mrLua(_W);
	
	std::string path = _W->resourcesPath;
	path.append("furnishings.lua");
	if (!mrLua.loadFile(path.c_str())) {
		W::log("Could not read furnishings.lua");
		return false;
	}
	lua_State *L = mrLua.LuaInstance;
	
	try {
		// Set defaults
		Furnishing::defaultColour      = mrLua.getvalue<const char *>("defaultColour");
		Furnishing::defaultHoverColour = mrLua.getvalue<const char *>("defaultHoverColour");
	} catch (MsgException &exc) {
		std::string s = "In furnishings.lua, could not get defaults. Error: "; s.append(exc.msg);
		W::log(s.c_str());
	}
	
	// Construct furnishingTypes map
	if (!mrLua.pushtable("furnishingTypes")) {
		W::log("In furnishings.lua, could not push the furnishingTypes table onto the stack");
		return false;
	}
	lua_pushnil(L);									// S: -1 nil; -2 table
	while (lua_next(L, -2) != 0) {					// S: -1 subtable; -2 key; -3 table
		if (!lua_istable(L, -1)) {			// If not a subtable, skip
			lua_pop(L, 1);
			continue;
		}
		const char *fType = lua_tostring(L, -2);
		struct furnishingInfo *fInfo = &Furnishing::furnishingTypes[fType];
		
		lua_getfield(L, -1, "colour");				// S: -1 colour; -2 subtable; -3 key; -4 table
		fInfo->col = lua_isstring(L, -1) ? lua_tostring(L, -1) : Furnishing::defaultColour;
		lua_pop(L, 1);
		
		lua_getfield(L, -1, "hoverColour");			// S: -1 colour; -2 subtable; -3 key; -4 table
		fInfo->hoverCol = lua_isstring(L, -1) ? lua_tostring(L, -1) : Furnishing::defaultHoverColour;
		lua_pop(L, 1);
		
		// Ground plans
		if (!mrLua.pushSubtable("groundplan")) {
			char s[100]; sprintf(s, "In furnishings.lua, could not find groundplan for '%s' type", fType);
			W::log(s);
			return false;
		}											// S: -1 groundplan; -2 subtable; -3 key; -4 table
		lua_pushnil(L);								// S: -1 nil; -2 groundplan; ...
		while (lua_next(L, -2)) {					// S: -1 value; -2 key; -3 groundplan
			intcoord c = { mrLua.getfield<int>(1), mrLua.getfield<int>(2) };
			fInfo->groundplan.push_back(c);
			lua_pop(L, 1);
		}											// S: -1 groundplan; -2 subtable; -3 key; -4 table
		lua_pop(L, 1);								// S: -1 subtable; -2 key; -3 table

		// Compatible Behaviours
		if (!mrLua.pushSubtable("compatibleBehaviours")) {
			char s[100]; sprintf(s, "In furnishings.lua, could not find compatibleBehaviours for '%s' type", fType);
			W::log(s);
			return false;
		}
		std::string s = "    "; s += fType; s += "\n      compatibleBehaviours: ";
		int n = 0;
		lua_pushnil(L);								// S: -1 nil; -2 compatibleBehaviours; -3 subtable; ...
		while (lua_next(L, -2)) {					// S: -1 interaction, -2 key; -3 compatibleBehaviours; ...
			if (!lua_isstring(L, -1)) {
				lua_pop(L, 1);
				continue;
			}
			std::string iType = lua_tostring(L, -1);
			fInfo->compatibleBehaviours.push_back(iType);
			n++;
			s += iType; s += ", ";
			lua_pop(L, 1);							// S: -1 key; -2 compatibleBehaviours
		}											// S: -1 compatibleBehaviours; -2 subtable; -3 key; -4 table
		lua_pop(L, 1);								// S: -1 subtable; -2 key; -3 table
		if (n > 0) s.erase(s.size() - 2);
		else s += "none";
		s += "\n";
		
		// Interaction Points
		if (!mrLua.pushSubtable("interactionPoints")) {	// S: -1 interactionPoints; -2 subtable; ...
			char s[100]; sprintf(s, "In furnishings.lua, could not find interactionPoints for '%s' type", fType);
			W::log(s);
			return false;
		}
		s += "      interactionPoints: ";
		n = 0;
		lua_pushnil(L);								// S: -1 nil; -2 interactionPoints; ...
		while (lua_next(L, -2)) {					// S: -1 point; -2 unittype; -3 interactionPoints; ...
			if (!lua_isstring(L, -2) || !lua_istable(L, -1)) {
				lua_pop(L, 1);
				continue;
			}
			try {
				std::string uType = lua_tostring(L, -2);
				intcoord c = {
					mrLua.getfield<int>(1),
					mrLua.getfield<int>(2)
				};
				fInfo->interactionPoints[uType] = c;
				char t[30]; sprintf(t, "%s => {%d,%d}, ", uType.c_str(), c.x, c.y);
				s += t;
				n++;
			} catch(MsgException &exc) {
				char s[150]; sprintf(s, "Invalid coordinates for compatibleBehaviour for '%s' type", fType);
				W::log(s);
				return false;
			}
			lua_pop(L, 1);							// S: -1 unittype; -2 interactionPoints; ...
		}											// S: -1 interactionPoints; -2 subtable; -3 key; -4 table
		if (n > 0) s.erase(s.size() - 2);
		else s += "none";
		W::log(s.c_str());
		
		lua_pop(L, 2);
	}

	Furnishing::initialized = true;
	return true;
}
