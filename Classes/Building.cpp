#include "Building.hpp"
#include "LuaHelper.hpp"
#include "Level.hpp"
#include "Furnishing.hpp"
#include "MrPaths.hpp"
#include "Behaviour.hpp"
#include "W.h"

// Initialize static members
std::map<std::string, struct buildingInfo> Building::buildingTypes;
W::Colour Building::defaultColour;
W::Colour Building::defaultHoverColour;
bool Building::initialized = false;

std::vector<std::map<Furnishing*, Unit*>::iterator> Building::_ind_array;

Building::Building(W::NavMap *_nm, const char *_type, std::vector<W::rect> *_plan, W::position &_pos, Level *_level) :
	navmap(_nm), type(_type), level(_level),
	clicked(false), time_hovered(0), hover(false), customerCount(0)
{
	pos  = _pos;
	plan = *_plan;
	bInfo = &Building::buildingTypes[type];	// Save ptr to properties for this building type
	
	W::Messenger::subscribeToEventType(W::EventType::LEVEL_LEFTMOUSEDOWN, W::Callback(&Building::receiveEvent, this));
	
	if (!navmap->isPassableUnder(this))
		throw W::Exception("Navmap was not passable under Building plan.");
	navmap->isolate(this);
}
Building::~Building()
{
	std::cout << "building destruct" << std::endl;
	navmap->unisolate(this);	// NOTE: W::NavMap::unisolate() is not yet functional
	W::Messenger::unsubscribeFromMouseEvents(this);
}

void Building::receiveEvent(W::Event *ev) {
	if (ev->type == W::EventType::LEVEL_LEFTMOUSEDOWN)
		level->openFurnishingPurchasingView(this);
}

void Building::update() {
	if (!buildingIsAtCapacity()) dequeueCustomer();
	
	// Queue dispatch
//	for (std::vector<Unit*>::iterator itQ = Q.begin(); itQ < Q.end(); ) {
//		Unit *u = *itQ;
//		Building::_ind_array.clear();
//		if (u->nextBehaviour.empty()) {
//			level->createBehaviour("despawn")->init(u);
//			itQ = Q.erase(itQ);
//			continue;
//		}
//		const char *beh = u->nextBehaviour.c_str();
//		// Get random Furnishing f ready for behaviour
//		std::map<Furnishing*, Unit*>::iterator itB;
//		for (itB = staffBindings.begin(); itB != staffBindings.end(); itB++)
//			if (itB->first->readyForBehaviour(beh) && itB->second->readyForBehaviour(beh))
//				Building::_ind_array.push_back(itB);
//		int nFound = (int) Building::_ind_array.size();
//		if (nFound == 0)
//			itQ++;
//		else {
//			itB = Building::_ind_array[W::randUpTo(nFound)];
//			Furnishing *f = itB->first;
//			Unit *s = itB->second;
//			level->createBehaviour(beh)->init(level, u, s, f);
//			u->nextBehaviour.clear();
//			itQ = Q.erase(itQ);
//		}
//	}
}

W::Colour& Building::col() {
	if (hover) { hover = false; return bInfo->hoverCol; }
	return bInfo->col;
}

void Building::getQueuePoint(W::position &p) {
	p = pos;
}
//void Building::getEntryPoint(int *_x, int *_y) {
//	// TODO: handle case where there are no doors
//	std::cout << "doors: " << doors.size() << std::endl;
//	door d = doors[W::randUpTo((int)doors.size())];
//	*_x = pos.x + d.pos.x;
//	*_y = pos.y + d.pos.y;
//	if      (d.orientation == Direction::DOWNWARD)  *_y += 1;
//	else if (d.orientation == Direction::UPWARD)    *_y -= 1;
//	else if (d.orientation == Direction::RIGHTWARD) *_x += 1;
//	else if (d.orientation == Direction::LEFTWARD)  *_x -= 1;
//}
bool Building::buildingIsAtCapacity() {
	return customerCount <= furnishings.size();
}
void Building::queueCustomer(CustomerBehaviour *_cb) {
	Q.push(_cb);
}
void Building::dequeueCustomer() {
	if (Q.size() == 0) return;
	CustomerBehaviour *cb = Q.front();
	Q.pop();
	cb->buildingAccessGranted();
}
void Building::customerEntering() { customerCount++; }
void Building::customerLeaving() { customerCount--; }

void Building::addFurnishing(Furnishing *f) {
	furnishings.push_back(f);
}
void Building::removeFurnishing(Furnishing *f) {
	for (std::vector<Furnishing *>::iterator it = furnishings.begin(); it != furnishings.end(); )
		if (*it == f) it = furnishings.erase(it);
		else ++it;
}
Furnishing* Building::getRandomAvailableFurnishing() {
	std::vector<Furnishing *> availables;
	int n = 0;
	for (std::vector<Furnishing*>::iterator it = furnishings.begin(); it < furnishings.end(); it++)
		if ((*it)->isAvailable())
			availables.push_back(*it), n++;
	if (n) return availables[W::randUpTo(n)];
	return NULL;
}

void Building::addShopKeeper(ShopKeeperBehaviour *_skb) {
	removeShopKeeper(_skb);
	shopKeeperBehaviours.push_back(_skb);
}
void Building::removeShopKeeper(ShopKeeperBehaviour *_skb) {
	for (std::vector<ShopKeeperBehaviour*>::iterator it = shopKeeperBehaviours.begin(); it < shopKeeperBehaviours.end(); )
		if (*it == _skb) it = shopKeeperBehaviours.erase(it);
		else it++;
}
ShopKeeperBehaviour* Building::getRandomAvailableShopKeeperBehaviour() {
	std::vector<ShopKeeperBehaviour*> availables;
	int n = 0;
	for (std::vector<ShopKeeperBehaviour*>::iterator it = shopKeeperBehaviours.begin(); it < shopKeeperBehaviours.end(); it++)
		if ((*it)->isAvailable())
			availables.push_back(*it), n++;
	if (n) return availables[W::randUpTo(n)];
	return NULL;
}

bool Building::objIsEntirelyInsideBuilding(W::MappedObj *obj) {
	using W::rect;
	using W::position;
	
	std::vector<position> o_coords, b_coords;
	
	// Populate vector of object's coords
	for (std::vector<rect>::iterator it = obj->plan.begin(); it < obj->plan.end(); it++) {
		int o_posx = obj->pos.x, o_posy = obj->pos.y;
		int r_posx = it->pos.x, r_posy = it->pos.y;
		int r_width = it->sz.width, r_height = it->sz.height;
		for (int i = o_posx + r_posx; i < o_posx + r_posx + r_width; i++)
			for (int j = o_posy + r_posy; j < o_posy + r_posy + r_height; j++)
				o_coords.push_back(position(i, j));
	}
	// And of the building's
	for (std::vector<rect>::iterator it = plan.begin(); it < plan.end(); it++) {
		int b_posx = pos.x, b_posy = pos.y;
		int r_posx = it->pos.x, r_posy = it->pos.y;
		int r_width = it->sz.width, r_height = it->sz.height;
		for (int i = b_posx + r_posx; i < b_posx + r_posx + r_width; i++)
			for (int j = b_posy + r_posy; j < b_posy + r_posy + r_height; j++)
				b_coords.push_back(position(i, j));
	}
	
	// Obj is entirely inside if all o_coords are present in b_coords
	for (std::vector<position>::iterator it = o_coords.begin(); it < o_coords.end(); it++) {
		bool found_in_b_coords = false;
		for (std::vector<position>::iterator itb = b_coords.begin(); itb < b_coords.end(); itb++)
			if (it->x == itb->x && it->y == itb->y)
				found_in_b_coords = true;
		if (!found_in_b_coords)
			return false;
	}
	return true;
}

bool Building::initialize() {
	if (Building::initialized) return true;
		
	W::log << "  Building::initialize() called..." << std::endl;
	LuaHelper mrLua;
	
	std::string path = MrPaths::luaPath;
	path.append("buildings.lua");
	if (!mrLua.loadFile(path.c_str())) {
		W::log << "Could not read buildings.lua" << std::endl;
		return false;
	}
	lua_State *L = mrLua.LuaInstance;
	
	try {
		// Set Building defaults
		Building::defaultColour      = strToColour(mrLua.getvalue<const char *>("defaultColour"));
		Building::defaultHoverColour = strToColour(mrLua.getvalue<const char *>("defaultHoverColour"));
	} catch (W::Exception &exc) {
		W::log << "In buildings.lua, could not get defaults. Error: " << exc.what() << std::endl;
	}
	
	// Construct Building::buildingTypes map
	if (!mrLua.pushtable("buildingTypes")) {
		W::log << "In buildings.lua, could not push the buildingTypes table onto the stack" << std::endl;
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
		if (lua_isstring(L, -1))
			bInfo->col = strToColour(lua_tostring(L, -1));
		else
			bInfo->col = Building::defaultColour;
		lua_pop(L, 1);
		
		lua_getfield(L, -1, "hoverColour");		// S: -1 colour; -2 buildingtype; -3 key; -4 buildingTypes
		if (lua_isstring(L, -1))
			bInfo->hoverCol = strToColour(lua_tostring(L, -1));
		else
			bInfo->hoverCol = Building::defaultHoverColour;
		lua_pop(L, 1);							// S: -1 buildingtype; -2 key; -3 buildingTypes

		if (!mrLua.pushSubtable("allowedFurnishings")){
			W::log << "In buildings.lua, couldn't find allowedFurnishings for '" << bType << "' type" << std::endl;
			return false;
		}
		std::string s = "    "; s += bType; s += ": allowedFurnishings: ";
		int n = 0;
		lua_pushnil(L);							// S: -1 nil; -2 allowedFurnishings; -3 buildingtype; ...
		while (lua_next(L, -2)) {				// S: -1 value; -2 key; -3 allowedFurnishings
			if (!lua_isstring(L, -1)) {
				lua_pop(L, 1);
				continue;
			}
			bInfo->allowedFurnishings.push_back(lua_tostring(L, -1));
			s += bInfo->allowedFurnishings.back(); s += ", ";
			n++;
			lua_pop(L, 1);						// S: -1 key; -2 allowedFurnishings; -3 buildingtype; ...
		}										// S: -1 allowedFurnishings; -2 buildingtype; ...
		lua_pop(L, 1);							// S: -1 buildingtype; -2 key; -3 buildingTypes
		if (n > 0) 
			s.erase(s.size() - 2);
		W::log << s << std::endl;
		
		lua_pop(L, 1);							// S: -1 key; -2 buildingTypes
	}

	Building::initialized = true;
	return true;
}
