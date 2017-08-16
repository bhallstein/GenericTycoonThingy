#include "Unit.hpp"
#include "Building.hpp"
#include "LevelState.hpp"
#include "LevelMap.hpp"
#include "Furnishing.hpp"
#include "MrPaths.hpp"
#include "W.h"
#include "LevelState.hpp"
#include "LevelView.hpp"
#include "Placeable.hpp"

W::EventType::T W::EventType::INTERRUPT_UNITPICKUP = W::Event::registerType();


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


/*** Unit ***/

Unit::Unit(LevelState *_ls, LevelMap *_lm, LevelView *_lv, W::NavMap *_nm, bool _placeableMode) :
	PlaceableManager(_ls, _lm, _lv, _nm, _placeableMode),
	mode(UnitMode::IDLE),
	hired(false)
{
	rct.setSz(W::size(1, 1));
	
	// Create DrawnUnit
	drawnUnit = new DrawnUnit(levelView);
	drawnUnit->setPosn(rct.pos);
}
Unit::~Unit()
{
	std::cout << "unit " << this << " destruct" << std::endl;
	using namespace W::EventType;
	W::Messenger::unsubscribeFromEventType(LV_LEFTMOUSEDOWN,  this);
	W::Messenger::unsubscribeFromEventType(LV_LEFTMOUSEUP,    this);
	W::Messenger::unsubscribeFromEventType(LV_RIGHTMOUSEDOWN, this);
	W::Messenger::unsubscribeFromEventType(LV_RIGHTMOUSEUP,   this);
	W::Messenger::unsubscribeFromEventType(LV_MOUSEMOVE,      this);
	delete drawnUnit;
}
void Unit::_setUp() {
	if (type == NO_TYPE)
		throw W::Exception("setUp() called on Unit with type NO_TYPE. Call setType() or deserialize first.");
	
	// Set typeInfo pointer
	using std::string;
	std::map<string,unitInfo*>::iterator it = unitTypeInfo.find(type);
	if (it == unitTypeInfo.end()) {
		string msg = string("Info for unit type \"") + type + "\" not found";
		throw W::Exception(msg);
	}
	typeInfo = Unit::unitTypeInfo[type];
	
	// Perform set-up for units constructed programmatically
	if (!deserialized) {
		// ...
	}
	
	// Set up state of DrawnUnit
	// e.g. du.setSpriteSet()...
}

W::EventPropagation::T Unit::mouseEvent(W::Event *ev) {
	using namespace W::EventType;
	
	W::EventPropagation::T v = W::EventPropagation::SHOULD_STOP;

	if (mode == UnitMode::ANIMATING) return v;
	if      (ev->type == LV_MOUSEMOVE) { /* hover = true; */ }
	else if (ev->type == LV_LEFTMOUSEDOWN) printDebugInfo();
	else if (ev->type == LV_RIGHTMOUSEDOWN) {
		if (typeInfo->isStaff) pickUp();
	}
	
	return v;
}

void Unit::update() {
	if (placeableMode) return;
	
	if (mode == UnitMode::IDLE) { }
	else if (mode == UnitMode::VOYAGING) { incrementLocation(); }
	//	else if (mode == ANIMATING) { incrementAnimation(); }
}


/*** Utility methods ***/

void Unit::wanderToRandomMapDestination() {
	W::position _dest(
		(int)W::randUpTo(levelMap->width()),
		(int)W::randUpTo(levelMap->height())
	);
	voyage(_dest);
		// TODO: call bhvr.failure if cannot voyage, or if obstacle encountered
}
void Unit::getDespawnPoint(W::position &p) {
	int w = navmap->width(), h = navmap->height(), n = W::randUpTo(2*w + 2*h - 4);
	if (n < w)            p.x = n,     p.y = 0;
	else if (n < 2*w)     p.x = n - w, p.y = h - 1;
	else if (n < 2*w + h) p.x = 0,     p.y = n - 2*w;
	else                  p.x = w - 1, p.y = n - (2*w + h);
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
//	int length = 60;
//	drawnUnit->incRot();
//	if (++frames_animated >= length) {
//		animation_finished = true;
//		mode = IDLE;
//	}
//}


/*** PlaceableManager overrides ***/

void Unit::placementLoopStarted() {
	drawnUnit->setOpac(0.2);		// Put DU in placement-loop mode
	if (Controller *c = controllerPtr())
		c->unitPickedUp(this);
}
void Unit::placementLoopUpdate() {
	W::position p(placeable.pos.x, placeable.pos.y, 0, 0);
	drawnUnit->setPosn(p);
}
void Unit::placementLoopCancelled() {
	if (typeInfo->isStaff) {
		// If were hiring a staff unit: destroy the object
		if (!hired) destroy();
	}
	// Otherwise, put the unit back where we found it
	else drawnUnit->setPosn(rct.pos);
	drawnUnit->setOpac(1);		// Put DrawnUnit back in normal mode
	if (Controller *c = controllerPtr())
		c->unitPutDown(this);
}
void Unit::placementLoopSucceeded() {
	if (!hired && typeInfo->isStaff) {
		// If hiring a staff unit: charge player
		levelState->addPlayerMoneys(-typeInfo->hireCost);
		hired = true;
	}
	drawnUnit->setPosn(rct.pos);	// Update DrawnUnit to new position
	drawnUnit->setOpac(1);			// Put D.U. back in normal mode
	// TODO: if voyaging, should recalculate route when placed
	// TODO: notify behaviour of placement event: may want to change associations to other TLOs, etc.
	if (Controller *c = controllerPtr())
		c->unitPutDown(this);
}
bool Unit::canPlace(const W::position &_pos) {
	return navmap->isPassableAt(_pos);
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
			pos.a = pos.b = 0;		// Arrived!
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
	for (LuaObj::_descendantmap::iterator it = o.descendants.begin(); it != o.descendants.end(); ++it)
		unitTypeInfo[it->first] = new unitInfo(it->second);
	
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
		mode == UnitMode::IDLE ? "IDLE" : "UNKNOWN"
	);
}


/* DrawnUnit impl */

Unit::DrawnUnit::DrawnUnit(LevelView *_lv) : lv(_lv)
{
	r = new W::DrawnRect(
		lv, W::position(), lv->convertGridToPixelCoords(W::size(1,1)), W::Colour::Black
	);
	lv->addDO(r);
}
Unit::DrawnUnit::~DrawnUnit() {
	lv->removeDO(r);
}
void Unit::DrawnUnit::setPosn(const W::position &p) {
	r->setPos(lv->convertGridToPixelCoords(p));
}
void Unit::DrawnUnit::setOpac(float x) {
	W::Colour c = r->col;
	c.a = x;
	r->setCol(c);
}
void Unit::DrawnUnit::incRot() {
	r->setRot(r->rot + 3);
}
