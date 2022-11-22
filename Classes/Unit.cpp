#include "Unit.hpp"
#include "Building.hpp"
#include "LevelMap.hpp"
#include "Furnishing.hpp"
#include "MrPaths.hpp"
#include "W.h"
#include "LevelMap.hpp"
#include "View__Game.hpp"
#include "Placeable.hpp"
#include "Serializer.hpp"


// unitInfo

unitInfo::unitInfo(LuaObj &o) {
  LuaObj *l;

  // Staffness (optional: def. no)
  l = &o["isStaff"];
  isStaff = (!l->isBool() ? false : l->bool_value());

  // Hire cost (optional: def. 0)
  l = &o["hireCost"];
  hireCost = (l->isNumber() ? l->number_value() : 0);
}


// Unit static properties

std::map<std::string, unitInfo*> Unit::unitTypeInfo;
Serializable::serialization_descriptor Unit::sd;
bool Unit::initialized = false;

std::map<std::string, W::Colour> unit_colors = {
  { "customer", W::Colour::Black },
  { "shopkeeper", {0,0.9882,0.7922,1} },
};


// Unit

Unit::Unit(LevelMap *_lm, View__Game *_lv, W::NavMap *_nm, bool _placeableMode) :
PlaceableManager(_lm, _lv, _nm, _placeableMode),
mode(UnitMode::IDLE),
hired(false)
{
  rct.size = {1, 1};

  // Create DrawnUnit
  drawnUnit = new DrawnUnit(view__game);
  drawnUnit->setPosn(rct.position);

  W::Messenger::subscribeInView(view__game, W::EventType::LMouseUp, W::Callback(&Unit::mouseEvent, this), &rct);
}
Unit::~Unit()
{
  W::Messenger::unsubscribeInView(view__game, W::EventType::LMouseUp, this);
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

W::EventPropagation::T Unit::mouseEvent(W::Event ev) {
  if (mode == UnitMode::ANIMATING) {
    return W::EventPropagation::ShouldStop;
  }

  if (ev.type == W::EventType::MouseMove) {
    //    hover = true;
  }

  else if (ev.type == W::EventType::LMouseUp) {
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

  drawnUnit->setPosn(rct.position);
}


/*** Utility methods ***/

bool Unit::wanderToRandomMapDestination() {
  W::v2i dest = levelMap->map__randomCoord();
  return voyage(dest);
}
bool Unit::voyage(W::v2i _dest) {
  dest = _dest;
  if (navmap->getRoute(rct.position.a, rct.position.b, dest.a, dest.b, route)) {
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
  W::v2f p(placeable.pos.a, placeable.pos.b);
  drawnUnit->setPosn(p);
}
void Unit::placementLoopCancelled() {
  if (typeInfo->isStaff) {
    // If we're hiring a staff unit: destroy the object
    if (!hired) destroy();
  }
  // Otherwise, put the unit back where we found it
  else drawnUnit->setPosn(rct.position);
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
  drawnUnit->setPosn(rct.position);  // Update DrawnUnit to new position
  drawnUnit->setOpac(1);      // Put D.U. back in normal mode

  // Remove route
  route.clear();
  mode = UnitMode::IDLE;

  // Notify controller of placement event
  if (Controller *c = controllerPtr()) {
    c->unitPutDown(this);
  }
}
bool Unit::canPlace(W::v2f _pos) {
  bool can_afford =
    !typeInfo->isStaff ||
    (typeInfo->isStaff && hired) ||
    (typeInfo->isStaff && !hired && levelMap->can_afford(typeInfo->hireCost));

  return can_afford && navmap->isPassableAt(W::v2i(_pos.a,
                                                   _pos.b));
}


/*** Voyaging-related ***/

bool arrived(W::v2i dest, W::v2f p, float step) {
  step += 0.01;

  W::v2f delta = p - dest;
  float mod_delta = delta.a*delta.a + delta.b*delta.b;

  return mod_delta <= (step * step);
}

bool Unit::incrementLocation() {
  float step = 0.10;
  W::v2f &pos = rct.position;
  W::v2i pos_next = route[0];

  if (arrived(pos_next, pos, step)) {
    route.erase(route.begin());
    pos = pos_next;
  }

  // Route completed
  if (route.size() == 0) {
    rct.position = dest;
    mode = UnitMode::IDLE;
    controllerPtr()->success(this);
    return true;
  }

  pos_next = route[0];

  // Inch toward pos_next
  bool diagonal = pos.a != pos_next.a && pos.b != pos_next.b;

  float a_diff = 0;
  float b_diff = 0;

  if (pos.a < pos_next.a)      { a_diff = step; }
  else if (pos.a > pos_next.a) { a_diff = -step; }
  if (pos.b < pos_next.b)      { b_diff = step; }
  else if (pos.b > pos_next.b) { b_diff = -step; }

  if (diagonal) {  // For diagonal traveling, normalise the motion vector by dividing by √2
    a_diff *= 0.707107;
    b_diff *= 0.707107;
  }
  pos.a += a_diff;
  pos.b += b_diff;

  return true;
}
bool Unit::inHinterland() {
  return (rct.position.a < HINTERLAND_WIDTH || rct.position.a >= navmap->width()-HINTERLAND_WIDTH ||
          rct.position.b < HINTERLAND_WIDTH || rct.position.b >= navmap->height()-HINTERLAND_WIDTH);
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
  for (auto it : o.descendants()) {
    unitTypeInfo[it.first] = new unitInfo(it.second);
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
  printf("\nUnit %p - x,y:%.1f,%.1f dest:%d,%d mode:%s\n\n",
         this, rct.position.a, rct.position.b, dest.a, dest.b,
         mode == UnitMode::VOYAGING ? "VOYAGING" :
         mode == UnitMode::ANIMATING ? "ANIMATING" :
         mode == UnitMode::IDLE ? "IDLE" : "UNKNOWN");
}


/* DrawnUnit impl */

Unit::DrawnUnit::DrawnUnit(View__Game *_lv) : lv(_lv)
{
  r = new W::Rectangle(lv,
                       W::v2f(),
                       lv->convertGridToPixelCoords({1,1}),
                       W::Colour::Black);
}
Unit::DrawnUnit::~DrawnUnit() {
  delete r;
}
void Unit::DrawnUnit::setPosn(W::v2f p) {
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
  r->setRot(r->rot + 3);
}

