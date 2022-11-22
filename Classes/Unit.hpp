#ifndef Unit_h
#define Unit_h

#include "types.hpp"
#include "PlaceableManager.hpp"
#include <map>

struct unitInfo {
  unitInfo(LuaObj &);
  bool isStaff;
  int hireCost;
};

class View__Game;
class Furnishing;
class Controller;

class Unit : public PlaceableManager {
public:
  Unit(LevelMap *, View__Game *, W::NavMap *, bool _placeableMode);
  ~Unit();
  void _setUp();
    // When a unit is constructed, we may not yet know its type, which is therefore
    // initially set to NO_TYPE.
    // The unit must then be "set up".
    //   setType(t)  - set the type manually
    //        - (not req’d if being deserialized)
    //   setUp() - perform setup (after type has been set)

    // Note: then also need to init() the Unit, since it is a PlaceableManager.

  // Event callbacks
  W::EventPropagation::T mouseEvent(W::Event);

  void update();

  // Utility methods
  bool wanderToRandomMapDestination();
  bool voyage(W::v2i);

  static bool initialize();   // Populate static unitTypes from units.lua
  static bool initialized;

  UID controller;

protected:
  UnitMode::T mode;
  Controller* controllerPtr() { return (Controller*) controller.get(); }
  bool hired;

  // PlaceableManager overrides
  void placementLoopStarted();
  void placementLoopUpdate();
  void placementLoopCancelled();
  void placementLoopSucceeded();
  bool canPlace(W::v2f);

  // Voyaging-related
  W::v2i dest;
  std::vector<W::v2i> route;
  bool incrementLocation();  // Move along route. Returns false if obstacle encountered.
  bool inHinterland();

  // Serialization
  static serialization_descriptor sd;
  virtual void getSDs(sdvec &vec) {
    PlaceableManager::getSDs(vec);
    vec.push_back(&Unit::sd);
  }
  void deserializeAdditionalProperties(LuaObj &o) { }
  void getAdditionalSerializedProperties(std::map<std::string, std::string> &m) { }

  // Type info
  struct unitInfo *typeInfo;
  static std::map<std::string, unitInfo*> unitTypeInfo;

  // Drawing
  class DrawnUnit;
  DrawnUnit *drawnUnit;

  // Other
  void printDebugInfo();
};


class Unit::DrawnUnit {
public:
  DrawnUnit(View__Game *);
  ~DrawnUnit();
  void setPosn(W::v2f);
  void setOpac(float x);
  void setCol(W::Colour);
  void incRot();

private:
  View__Game *lv;
  W::Rectangle *r;
};

#endif
