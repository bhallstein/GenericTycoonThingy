#include "Building.hpp"
#include "LevelState.hpp"
#include "LevelView.hpp"
#include "Furnishing.hpp"
#include "MrPaths.hpp"
#include "Behaviour.hpp"
#include "W.h"

/* buildingInfo constructor impl */

buildingInfo::buildingInfo(LuaObj &o) {
	LuaObj *l;
	
	// Allowed furnishings
	l = &o["allowedFurnishings"];
	for (LuaObj::_descendantmap::iterator it = l->descendants.begin(); it != l->descendants.end(); ++it) {
		LuaObj &d = it->second;
		if (!d.isString()) throw W::Exception("Error creating buildingInfo: allowedFurnishing entries must be strings");
		allowedFurnishings.push_back(d.str_value);
	}
}


/* Building impl */

std::map<std::string, buildingInfo*> Building::buildingTypeInfo;
bool Building::initialized = false;
std::vector<std::map<Furnishing*, Unit*>::iterator> Building::_ind_array;

Building::Building(LevelState *_ls, LevelMap *_lm, LevelView *_lv, W::NavMap *_nm, const char *_type, std::vector<W::rect> *_plan, W::position &_pos) :
	TLO(_ls, _lm, _lv, _nm),
	type(_type)
{
	rct.setPos(_pos);
//	bInfo = &Building::buildingTypes[type];	// Save ptr to properties for this building type
	
	W::Messenger::subscribeToEventType(W::EventType::LV_LEFTMOUSEDOWN, W::Callback(&Building::mouseEvent, this));
	
	if (!navmap->isPassableUnder(rct))
		throw W::Exception("Navmap was not passable under Building plan.");
	navmap->isolate(rct);
}
Building::~Building()
{
	std::cout << "building destruct" << std::endl;
	navmap->unisolate(rct);		// NOTE: W::NavMap::unisolate() is not yet functional
	W::Messenger::unsubscribeFromMouseEvents(this);
}

W::EventPropagation::T Building::mouseEvent(W::Event *ev) {
	if (ev->type == W::EventType::LV_LEFTMOUSEDOWN) {
//		level->openFurnishingPurchasingView(this);
	}
	return W::EventPropagation::SHOULD_CONTINUE;
}

bool Building::initialize() {
	if (Building::initialized) return true;
		
	// 1. Get furnishingTypeInfo LuaObj
	
	std::string path = MrPaths::resourcesPath + "Lua/buildings.lua";
	lua_State *L;
	if (!luaLoad(path, &L)) {
		W::log << "Could not read buildings.lua" << std::endl;
		return false;
	}
	lua_getglobal(L, "buildingTypes"); LuaObj o(L);
	lua_close(L);
	if (!o.isTable()) {
		W::log << "Could not get buildingTypes table from buildings.lua" << std::endl;
		return false;
	}
	LuaObj::_descendantmap &desc = o.descendants;
	for (LuaObj::_descendantmap::iterator it = desc.begin(); it != desc.end(); ++it)
		buildingTypeInfo[it->first] = new buildingInfo(it->second);
	
	// 2. Set up Serialization Descriptor
	// ...

	Building::initialized = true;
	return true;
}
