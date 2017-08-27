/*
 * Generic Tycoon Thingy
 *
 * =================
 *  Building.cpp
 * =================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#include "Building.hpp"
#include "LevelState.hpp"
#include "LevelView.hpp"
#include "Furnishing.hpp"
#include "MrPaths.hpp"
#include "W.h"

/* buildingInfo constructor impl */

buildingInfo::buildingInfo(LuaObj &o) {
	LuaObj *l;
	
	// Allowed furnishings
	l = &o["allowedFurnishings"];
	for (LuaObj::_descendantmap::iterator it = l->descendants.begin(); it != l->descendants.end(); ++it) {
		LuaObj &d = it->second;
		if (!d.isString()) throw W::Exception("Error creating buildingInfo: allowedBuilding entries must be strings");
		allowedFurnishings.push_back(d.str_value);
	}
}


/* Building: static properties */

std::map<std::string, buildingInfo*> Building::buildingTypeInfo;
//Serializable::serialization_descriptor Building::sd;
bool Building::initialized = false;


/*** Building ***/

Building::Building(LevelState *_ls, LevelMap *_lm, LevelView *_lv, W::NavMap *_nm) :
	TLO(_ls, _lm, _lv, _nm)
{
	drawnBuilding = new DrawnBuilding(levelView, W::position());
	
//	if (!navmap->isPassableUnder(rct)) {
//		throw W::Exception("Navmap was not passable under Building plan.");
//	}
}
Building::~Building()
{
	std::cout << "building destruct" << std::endl;
	navmap->unisolate(rct);		// NOTE: W::NavMap::unisolate() is not yet functional
	delete drawnBuilding;
}

void Building::_setUp() {
	if (type == NO_TYPE) {
		throw W::Exception("setUp() called on Building with type NO_TYPE. Call setType() or deserialize first.");
	}
	
	// Set typeInfo pointer
	auto it = buildingTypeInfo.find(type);
	if (it == buildingTypeInfo.end()) {
		throw W::Exception(std::string("Info for building type \"") + type + "\" not found");
	}
	typeInfo = Building::buildingTypeInfo[type];
	
	// Perform set-up for buildings constructed programmatically
	if (!deserialized) {
		// ...
	}
	
	// Set up state of DrawnFurnishing
}


bool Building::initialize() {
	if (Building::initialized) return true;
	
	// 1. Get buildingTypeInfo LuaObj
	
	std::string path = MrPaths::resourcesPath + "Data/Object info/buildings.lua";
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

void Building::setPos(const W::position &_pos) {
	rct.pos = _pos;
	drawnBuilding->setPosn(_pos);
}



// DrawnBuilding impl

Building::DrawnBuilding::DrawnBuilding(LevelView *_lv, const W::position &_pos) : lv(_lv)
{
	r = new W::DRect(
		_lv, _pos, _lv->convertGridToPixelCoords(W::size(5,3)), W::Colour::TransparentBlack
	);
}
Building::DrawnBuilding::~DrawnBuilding()
{
	delete r;
}
void Building::DrawnBuilding::setPosn(const W::position &_pos) {
	r->setPos(lv->convertGridToPixelCoords(_pos));
}
