/*
 * Generic Tycoon Thingy
 *
 * ================
 *  Building.hpp
 * ================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
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

class LevelState;
class Unit;
class Furnishing;

struct buildingInfo {
  buildingInfo(LuaObj &);
  std::vector<std::string> allowedFurnishings;
};

class Building : public TLO {
public:
  Building(LevelMap *, LevelView *, W::NavMap *);
  ~Building();

  void _setUp();  // See note in Unit.hpp on creation & setup

  void update() { }

  static bool initialize();
  static bool initialized;

  void setPos(const W::position &);

  void addShopkeeper(UID);
  void removeShopkeeper(UID);
  std::vector<UID> shopkeeper__getAll() { return activeShopkeepers; }

  bool contains_point(W::position);
  W::position centrePoint();

protected:
  std::vector<W::position> groundplan;
  std::vector<W::position> doors;

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

  // Active shopkeeper controllers
  std::vector<UID> activeShopkeepers;
};


class Building::DrawnBuilding {
public:
  DrawnBuilding(LevelView *, W::position pos);
  ~DrawnBuilding();
  void setPos(W::position);
  void setGroundplan(std::vector<W::position>);
  void setCol(W::Colour);
private:
  W::position pos;
  W::Colour col;
  std::vector<W::position> groundplan;
  std::vector<W::DRect*> rects;
  LevelView *lv;
};

#endif

