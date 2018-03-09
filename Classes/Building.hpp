/*
 * Generic Tycoon Thingy
 *
 * ===============
 *  Building.hpp
 * ===============
 *
 * Copyright (C) 2012 - Ben Hallstein
 * All rights reserved
 *
 */

/*
 * Building.hpp
 *
 * Building is a TLO subclass, since Buildings are saved in levels
 * and hence need to be serializable.
 *
 */

#ifndef BUILDING_H
#define BUILDING_H

#include <iostream>
#include <map>
#include <string>
#include <queue>

#include "types.hpp"
#include "TLO.hpp"

class Unit;
class Furnishing;
class Controller;

struct buildingInfo {
  buildingInfo(LuaObj &);
  std::vector<std::string> allowedFurnishings;
};

class Building : public TLO {
public:
  Building(LevelMap *, View__Game *, W::NavMap *);
  ~Building();

  void _setUp();  // See note in Unit.hpp on creation & setup

  void update() { }

  static bool initialize();
  static bool initialized;

  void setPos(W::v2f);

  bool contains_point(W::v2f);
  W::v2i centrePoint();

  W::EventPropagation::T mouseEvent(W::Event);

  void remove_controller(UID);
  void add_controller(UID);
  std::vector<UID> get_operating_controllers();

protected:
  std::vector<W::v2i> groundplan;
  std::vector<W::fRect> groundplan_rects;  // For event subscription :/
  std::vector<W::v2i> doors;

  // Serialization
  virtual void getSDs(sdvec &vec) {
    TLO::getSDs(vec);
    vec.push_back(&Building::sd);
  }
  static serialization_descriptor sd;
  void deserializeAdditionalProperties(LuaObj &o) { }
  void getAdditionalSerializedProperties(std::map<std::string, std::string> &m) { }

  // Type info
  struct buildingInfo *typeInfo;
  static std::map<std::string, buildingInfo*> buildingTypeInfo;

  // Drawing
  class DrawnBuilding;
  DrawnBuilding *drawnBuilding;

  // Controllers operating in the building
  // (necessary because controllers cannot be queried positionally)
  std::vector<UID> operating_controllers;
};


class Building::DrawnBuilding {
public:
  DrawnBuilding(View__Game *, W::v2f pos);
  ~DrawnBuilding();
  void setPos(W::v2f);
  void setGroundplan(std::vector<W::v2i>);
  void setCol(W::Colour);
private:
  W::v2f pos;
  W::Colour col;
  std::vector<W::v2f> groundplan;
  std::vector<W::Rectangle*> rects;
  View__Game *lv;
};

#endif

