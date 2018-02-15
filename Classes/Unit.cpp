/*
 * Generic Tycoon Thingy
 *
 * ============
 *  Unit.cpp
 * ============
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#include "Unit.hpp"
#include "Building.hpp"
#include "LevelMap.hpp"
#include "Furnishing.hpp"
#include "MrPaths.hpp"
#include "W.h"
#include "LevelMap.hpp"
#include "LevelView.hpp"
#include "Placeable.hpp"
#include "Serializer.hpp"


/* unitInfo constructor impl */

unitInfo::unitInfo(LuaObj &o) {
  LuaObj *l;

  // Staffness (optional: def. no)
  l = &o["isStaff"];
  isStaff = (!l->isBool() ? false : l->bool_value);

  // Hire cost (optional: def. 0)
  l = &o["hireCost"];
  hireCost = (l->isNumber() ? l->number_value : 0);
}


/* Unit: static properties */

std::map<std::string, unitInfo*> Unit::unitTypeInfo;
Serializable::serialization_descriptor Unit::sd;
bool Unit::initialized = false;


std::map<std::string, W::Colour> unit_colors = {
  { "customer", W::Colour::Black },
  { "shopkeeper",    {0,0.9882,0.7922,1} },
};


/*** Unit ***/

Unit::Unit(LevelMap *_lm, LevelView *_lv, W::NavMap *_nm, bool _placeableMode) :
PlaceableManager(_lm, _lv, _nm, _placeableMode),
mode(UnitMode::IDLE),
hired(false)
{
  rct.setSz(W::size(1, 1));

  // Create DrawnUnit
  drawnUnit = new DrawnUnit(levelView);
  drawnUnit->setPosn(rct.pos);

  W::Messenger::subscribeInView(levelView, W::EventType::LMouseUp, W::Callback(&Unit::mouseEvent, this), &rct);
}
Unit::~Unit()
{
  W::Messenger::unsubscribeInView(levelView, W::EventType::LMouseUp, this);
  delete drawnUnit;
}
void Unit::_setUp() {
  if (type == NO_TYPE) {
    throw W::Exception("setUp() called on Unit with type NO_TYPE. Call setType() or deserialize first.");
  }

  // Set typeInfo pointer
  using std::string;
  auto it = unitTypeInfo.find(type);
  if (it == unitTypeInfo.end()) {
    string msg = string("Info for unit type \"") + type + "\" not found";
    throw W::Exception(msg);
  }
  typeInfo = Unit::unitTypeInfo[type];

  // Set state of drawnUnit
  std::cout << type << "\n";
  auto col = unit_colors.find(type);
  drawnUnit->setCol(col == unit_colors.end() ? W::Colour::Purple : col->second);

  // Perform set-up for units constructed programmatically
  if (!deserialized) {
    // ...
  }
}

W::EventPropagation::T Unit::mouseEvent(W::Event *ev) {
  if (mode == UnitMode::ANIMATING) {
    return W::EventPropagation::ShouldStop;
  }

  if (ev->type == W::EventType::MouseMove) {
    //    hover = true;
  }

  else if (ev->type == W::EventType::LMouseUp) {
    if (typeInfo->isStaff) {
      pickUp();

      // Dispatch interrupt event (used by controller)
      //      W::Event ev_interrupt(W::EventType::INTERRUPT_UNITPICKUP);
      //      ev_interrupt._payload = this;
      //      W::Messenger::dispatchEvent(&ev_interrupt);
    }
  }

  return W::EventPropagation::ShouldStop;
}

void Unit::update() {
  if (placeableMode) return;

  if (mode == UnitMode::IDLE) { }
  else if (mode == UnitMode::VOYAGING) { incrementLocation(); }
  //  else if (mode == ANIMATING) { incrementAnimation(); }
}


/*** Utility methods ***/

bool Unit::wanderToRandomMapDestination() {
  W::position dest = levelMap->map__randomCoord();
  return voyage(dest);
}
bool Unit::voyage(const W::position &_dest) {
  dest = _dest;
  if (navmap->getRoute(rct.pos.x, rct.pos.y, dest.x, dest.y, route)) {
    mode = UnitMode::VOYAGING;
    return true;
  }
  return false;
}
//void Unit::incrementAnimation() {
//  int length = 60;
//  drawnUnit->incRot();
//  if (++frames_animated >= length) {
//    animation_finished = true;
//    mode = IDLE;
//  }
//}


/*** PlaceableManager overrides ***/

void Unit::placementLoopStarted() {
  drawnUnit->setOpac(0.2);    // Put DU in placement-loop mode
  if (Controller *c = controllerPtr()) {
    c->unitPickedUp(this);
  }
}
void Unit::placementLoopUpdate() {
  W::position p(placeable.pos.x, placeable.pos.y, 0, 0);
  drawnUnit->setPosn(p);
}
void Unit::placementLoopCancelled() {
  if (typeInfo->isStaff) {
    // If we're hiring a staff unit: destroy the object
    if (!hired) destroy();
  }
  // Otherwise, put the unit back where we found it
  else drawnUnit->setPosn(rct.pos);
  drawnUnit->setOpac(1);    // Put DrawnUnit back in normal mode
  if (Controller *c = controllerPtr())
    c->unitPutDown(this);
}
void Unit::placementLoopSucceeded() {
  // If hiring a staff unit for the first time: charge player
  if (typeInfo->isStaff && !hired) {
    levelMap->addPlayerMoneys(-typeInfo->hireCost);
    hired = true;
  }
  drawnUnit->setPosn(rct.pos);  // Update DrawnUnit to new position
  drawnUnit->setOpac(1);      // Put D.U. back in normal mode

  // Remove route
  route.clear();
  mode = UnitMode::IDLE;

  // Notify controller of placement event
  if (Controller *c = controllerPtr()) {
    c->unitPutDown(this);
  }
}
bool Unit::canPlace(const W::position &_pos) {
  bool can_afford =
    !typeInfo->isStaff ||
    (typeInfo->isStaff && hired) ||
    (typeInfo->isStaff && !hired && levelMap->can_afford(typeInfo->hireCost));

  return can_afford && navmap->isPassableAt(_pos);
}


/*** Voyaging-related ***/

bool Unit::incrementLocation() {
  if (rct.pos == dest) {
    mode = UnitMode::IDLE;
    controllerPtr()->success(this);
    return true;
  }

  W::position &pos = rct.pos;

  float step = 0.10;
  float a_diff = 0, b_diff = 0;

  bool diagonal = (pos.a != 0 && pos.b != 0);

  // Check if we've reached the next loc. This happens:
  //  - for linears, when |a| < step or |b| < step
  //  - for diagonals, when a^2 + b^2 < 2 * step^2
  bool reached_next = false;
  if (diagonal) {
    reached_next = pos.a*pos.a + pos.b*pos.b < 2*step*step;
  }
  else {
    float t = (pos.a == 0 ? pos.b : pos.a);
    reached_next = t*t < step*step;
  }

  if (reached_next) {
    if (route.empty())
      pos.a = pos.b = 0;    // Arrived!
    else {
      // Get next in route
      W::position next_pos = route[0];
      if (navmap->isPassableAt(next_pos)) {
        W::position prev_pos = pos;
        pos = next_pos;
        pos.a = prev_pos.x == pos.x ? 0 : prev_pos.x < pos.x ? -1 : 1;
        pos.b = prev_pos.y == pos.y ? 0 : prev_pos.y < pos.y ? -1 : 1;
        route.erase(route.begin());
      }
      else {
        return navmap->getRoute(pos.x, pos.y, dest.x, dest.y, route);
      }
    }
  }
  else {
    // Decrement a & b toward current x,y position
    if (pos.a <= -step)     a_diff = step;
    else if (pos.a >= step) a_diff = -step;
    if (pos.b <= -step)     b_diff = step;
    else if (pos.b >= step) b_diff = -step;
    if (diagonal) a_diff *= 0.7071, b_diff *= 0.7071; // For diagonal traveling, normalise the motion vector by dividing by √2
    pos.a += a_diff, pos.b += b_diff;
  }
  drawnUnit->setPosn(rct.pos);
  return true;
}
bool Unit::inHinterland() {
  return (
          rct.pos.x < HINTERLAND_WIDTH || rct.pos.x >= navmap->width()-HINTERLAND_WIDTH ||
          rct.pos.y < HINTERLAND_WIDTH || rct.pos.y >= navmap->height()-HINTERLAND_WIDTH
          );
}


/*** Unit: static initialization ***/

bool Unit::initialize() {
  if (Unit::initialized) return true;

  /* 1. Get LuaObj & save unitTypeInfo map */
  std::string path = MrPaths::resourcesPath + "Data/Object info/units.lua";
  lua_State *L;
  if (!luaLoad(path, &L)) {
    W::log << "Could not read units.lua" << std::endl;
    return false;
  }
  lua_getglobal(L, "unitTypes");
  LuaObj o(L);
  lua_close(L);
  if (!o.isTable()) {
    W::log << "Could not get unitTypes table from units.lua" << std::endl;
    return false;
  }
  for (auto it = o.descendants.begin(); it != o.descendants.end(); ++it) {
    unitTypeInfo[it->first] = new unitInfo(it->second);
  }

  /* 2. Set up Serialization Descriptor */
  sd["dest"]  = makeSerializer(&Unit::dest);
  sd["route"] = makeSerializer(&Unit::route);
  sd["hired"] = makeSerializer(&Unit::hired);
  sd["mode"]  = makeSerializer(&Unit::mode);
  sd["controller"] = makeSerializer(&Unit::controller);

  return Unit::initialized = true;
}


/* Unit: Other */

void Unit::printDebugInfo() {
  printf("\nUnit %p - x,y:%d,%d a,b:%.1f,%.1f dest:%d,%d mode:%s\n\n",
         this, rct.pos.x, rct.pos.y, rct.pos.a, rct.pos.b, dest.x, dest.y,
         mode == UnitMode::VOYAGING ? "VOYAGING" :
         mode == UnitMode::ANIMATING ? "ANIMATING" :
         mode == UnitMode::IDLE ? "IDLE" : "UNKNOWN");
}


/* DrawnUnit impl */

Unit::DrawnUnit::DrawnUnit(LevelView *_lv) : lv(_lv)
{
  r = new W::DRect(lv,
                   W::position(),
                   lv->convertGridToPixelCoords(W::size(1,1)),
                   W::Colour::Black);
}
Unit::DrawnUnit::~DrawnUnit() {
  delete r;
}
void Unit::DrawnUnit::setPosn(W::position p) {
  r->setPos(lv->convertGridToPixelCoords(p));
}
void Unit::DrawnUnit::setOpac(float x) {
  W::Colour c = r->col;
  c.a = x;
  setCol(c);
}
void Unit::DrawnUnit::setCol(W::Colour c) {
  r->setCol(c);
}
void Unit::DrawnUnit::incRot() {
  r->setRot(r->rotation + 3);
}

