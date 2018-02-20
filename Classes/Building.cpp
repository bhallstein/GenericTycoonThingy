/*
 * Generic Tycoon Thingy
 *
 * ===============
 *  Building.cpp
 * ===============
 *
 * Copyright (C) 2012 - Ben Hallstein
 * All rights reserved
 *
 */

#include "Building.hpp"
#include "View__Game.hpp"
#include "Furnishing.hpp"
#include "MrPaths.hpp"
#include "W.h"
#include "Serializer.hpp"
#include "Messenger.h"

/* buildingInfo constructor impl */

buildingInfo::buildingInfo(LuaObj &o) {
	LuaObj *l;
	
	// Allowed furnishings
	l = &o["allowedFurnishings"];
  for (auto it : l->descendants()) {
		LuaObj &d = it.second;
		if (!d.isString()) throw W::Exception("Error creating buildingInfo: allowedBuilding entries must be strings");
		allowedFurnishings.push_back(d.str_value());
	}
}


/* Building: static properties */

std::map<std::string, buildingInfo*> Building::buildingTypeInfo;
Serializable::serialization_descriptor Building::sd;
bool Building::initialized = false;


std::map<std::string, W::Colour> building_colors;


/*** Building ***/

Building::Building(LevelMap *_lm, View__Game *_lv, W::NavMap *_nm) :
	TLO(_lm, _lv, _nm)
{
  if (building_colors.size() == 0) {
    building_colors = {
      { "home",    {0,0,0,0.85} },
      { "barber",  {0,0,0,0.4} },
      { "pieshop", {0,0,0,0.6} },
    };
  }

  drawnBuilding = new DrawnBuilding(view__game, W::v2f());

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

  // Create groundplan_rects
  groundplan_rects.clear();
  for (auto p : groundplan) {
    W::v2f r_pos = {
      p.a + rct.position.a,
      p.b + rct.position.b,
    };
    W::fRect r = { r_pos, {1,1} };
    groundplan_rects.push_back(r);
  }

  // Subscribe to mouse clicks
  for (auto &r : groundplan_rects) {
    W::Messenger::subscribeInView(view__game,
                                  W::EventType::LMouseUp,
                                  W::Callback(&Building::mouseEvent, this),
                                  &r);
  }
	
	// Set up state of DrawnBuilding
  drawnBuilding->setPos(rct.position);
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
    p.a += rct.position.a;
    p.b += rct.position.b;
  }
  navmap->isolate(groundplan_levelcoords);

  auto doors_levelcoords = doors;
  for (auto &p : doors_levelcoords) {
    p.a += rct.position.a;
    p.b += rct.position.b;
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
  for (auto it : o.descendants()) {
		buildingTypeInfo[it.first] = new buildingInfo(it.second);
  }
	
	// 2. Set up Serialization Descriptor
  sd["groundplan"] = makeSerializer(&Building::groundplan);
  sd["doors"] = makeSerializer(&Building::doors);

	Building::initialized = true;
	return true;
}

void Building::setPos(W::v2f _pos) {
	rct.position = _pos;
	drawnBuilding->setPos(_pos);
}

bool Building::contains_point(W::v2f _p) {
  W::v2i p = { int(_p.a), int(_p.b) };
  for (auto it : groundplan) {
    W::v2i gp_lv = {
      it.a + int(rct.position.a),
      it.b + int(rct.position.b),
    };
    if (gp_lv == p) {
      return true;
    }
  }
  return false;
}

W::v2i Building::centrePoint() {
  // Return groundplan item closest to average_position
  W::v2i gp_sum;
  for (auto gp : groundplan) {
    gp_sum += gp;
  }
  float avg_x = float(gp_sum.a) / groundplan.size();
  float avg_y = float(gp_sum.b) / groundplan.size();

  float min_dist = 10000000000.f;
  W::v2i best = groundplan[0];
  for (auto gp : groundplan) {
    float dist = (avg_x - gp.a)*(avg_x - gp.a) + (avg_y - gp.b)*(avg_y - gp.b);
    if (dist < min_dist) {
      min_dist = dist;
      best = gp;
    }
  }

  best += W::v2i(
    rct.position.a,
    rct.position.b
  );
  return best;
}

W::EventPropagation::T Building::mouseEvent(W::Event *ev) {
  static W::Event *btn_ev = NULL;
  if (btn_ev == NULL) {
    btn_ev = new W::Event(W::EventType::ButtonClick);
    btn_ev->_payload = new std::string("open_furnishing_purchasing_view");
  }

  W::Messenger::dispatchEvent(btn_ev);
  return W::EventPropagation::ShouldStop;
}

void Building::add_controller(UID c) {
  operating_controllers.push_back(c);
}
void Building::remove_controller(UID c) {
  std::vector<UID> out;
  std::copy_if(operating_controllers.begin(), operating_controllers.end(), std::back_inserter(out), [=](UID _c) {
    return c != _c;
  });
  operating_controllers = out;
}
std::vector<UID> Building::get_operating_controllers() {
  return operating_controllers;
}



// DrawnBuilding impl

Building::DrawnBuilding::DrawnBuilding(View__Game *_lv, W::v2f _pos) :
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

void Building::DrawnBuilding::setGroundplan(std::vector<W::v2i> _groundplan) {
  for (auto it : rects) {
    delete it;
  }
  rects.clear();

  W::v2f grid_element_size = lv->convertGridToPixelCoords(W::v2f(1.,1.));
  for (size_t i=0; i < _groundplan.size(); ++i) {
    rects.push_back(new W::Rectangle(lv,
                                     lv->convertGridToPixelCoords(_groundplan[i]) + pos,
                                     grid_element_size,
                                     col));
  }
}

void Building::DrawnBuilding::setPos(W::v2f _pos) {
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
