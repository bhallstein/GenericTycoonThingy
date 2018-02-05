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
#include "Serializer.hpp"

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
Serializable::serialization_descriptor Building::sd;
bool Building::initialized = false;


std::map<std::string, W::Colour> building_colors = {
  { "home",    {0,0,0,0.8} },
  { "barber",  W::Colour::Red },
  { "pieshop", W::Colour::Yellow },
};


/*** Building ***/

Building::Building(LevelMap *_lm, LevelView *_lv, W::NavMap *_nm) :
	TLO(_lm, _lv, _nm)
{
  drawnBuilding = new DrawnBuilding(levelView, W::position());
	
//	if (!navmap->isPassableUnder(rct)) {
//		throw W::Exception("Navmap was not passable under Building plan.");
//	}
}
Building::~Building()
{
	std::cout << "building destruct" << std::endl;
//  navmap->unisolate(rct);    // NOTE: W::NavMap::unisolate() is not yet functional
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
	
	// Set up state of DrawnBuilding
  drawnBuilding->setPos(rct.pos);
  drawnBuilding->setGroundplan(groundplan);
  auto col = building_colors.find(type);
  if (col == building_colors.end()) {
    drawnBuilding->setCol(W::Colour::Pink);
  }
  else {
    drawnBuilding->setCol(col->second);
  }

  // Add to navmap
  auto groundplan_levelcoords = groundplan;
  for (auto &p : groundplan_levelcoords) {
    p += rct.pos;
  }
  navmap->isolate(groundplan_levelcoords);

  auto doors_levelcoords = doors;
  for (auto &p : doors_levelcoords) {
    p += rct.pos;
  }
  for (auto it = doors_levelcoords.begin(); it < doors_levelcoords.end(); it += 2) {
    navmap->createConnection(*it, *(it + 1));
  }
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
  for (auto it = desc.begin(); it != desc.end(); ++it) {
		buildingTypeInfo[it->first] = new buildingInfo(it->second);
  }
	
	// 2. Set up Serialization Descriptor
  sd["groundplan"] = makeSerializer(&Building::groundplan);
  sd["doors"] = makeSerializer(&Building::doors);

	Building::initialized = true;
	return true;
}

void Building::setPos(const W::position &_pos) {
	rct.pos = _pos;
	drawnBuilding->setPos(_pos);
}

void Building::addShopkeeper(UID sk) {
	activeShopkeepers.push_back(sk);
}
void Building::removeShopkeeper(UID sk_to_remove) {
	for (auto it = activeShopkeepers.begin(); it != activeShopkeepers.end(); ) {
		UID sk = *it;
		if (sk == sk_to_remove) {
			it = activeShopkeepers.erase(it);
		}
		else {
			++it;
		}
	}
}



// DrawnBuilding impl

Building::DrawnBuilding::DrawnBuilding(LevelView *_lv, W::position _pos) :
  lv(_lv),
  col(W::Colour::SkyBlue)
{
  setPos(_pos);
}

Building::DrawnBuilding::~DrawnBuilding()
{
  for (auto it : rects) {
    delete it;
  }
  rects.clear();
}

void Building::DrawnBuilding::setGroundplan(std::vector<W::position> _groundplan) {
  for (auto it : rects) {
    delete it;
  }
  rects.clear();

  W::size grid_element_size = lv->convertGridToPixelCoords(W::size(1,1));
  for (size_t i=0; i < _groundplan.size(); ++i) {
    rects.push_back(new W::DRect(lv,
                                 lv->convertGridToPixelCoords(_groundplan[i]) + pos,
                                 grid_element_size,
                                 col));
  }
}

void Building::DrawnBuilding::setPos(W::position _pos) {
  pos = lv->convertGridToPixelCoords(_pos);

  for (size_t i=0; i < groundplan.size(); ++i) {
    auto pl = groundplan[i];
    auto r = rects[i];

    r->setPos(pos + lv->convertGridToPixelCoords(pl));
  }
}

void Building::DrawnBuilding::setCol(W::Colour _c) {
  col = _c;

  for (auto r : rects) {
    r->setCol(_c);
  }
}
