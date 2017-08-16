#include "Behaviour.hpp"
#include "Unit.hpp"
#include "Building.hpp"
#include "Level.hpp"
#include "Furnishing.hpp"

Behaviour::Behaviour(const char *_type) : type(_type) 
{
	if (strcmp(_type, "despawn") == 0)          b = new DespawnBehaviour();
	else if (strcmp(_type, "seekhaircut") == 0) b = new SeekHaircutBehaviour();
	else if (strcmp(_type, "havehaircut") == 0) b = new HaveHaircutBehaviour();
	else if (strcmp(_type, "seekpie") == 0)     b = new SeekPieBehevaiour();
	else if (strcmp(_type, "piesale") == 0)     b = new PieSaleBehaviour();
	else throw MsgException("Behaviour with invalid type created");
}
Behaviour::~Behaviour()
{
	std::cout << "Behaviour \"" << type << "\" destruct" << std::endl;
	delete b;
}
void Behaviour::update() { b->update(); }
bool Behaviour::destroyed() { return b->destroyed; }
void Behaviour::destroy() { b->destroyed = true; }
/* Initialiser passthroughs */
void Behaviour::init(Unit *u) { b->init(u); }
void Behaviour::init(Level *l, Unit *u) { b->init(l, u); } 
void Behaviour::init(Unit *u1, Unit *u2, Furnishing *f) { b->init(u1, u2, f); }
void Behaviour::init(Level *l, Unit *u1, Unit *u2, Furnishing *f) { b->init(l, u1, u2, f); }


void DespawnBehaviour::init(Unit *_u) {
	unit = _u;
	initialized = true;
}
void DespawnBehaviour::_update() {
	if (stage == -1) {		// Waiting
		if (++frames_waited >= WAIT_PERIOD) stage++;
	}
	else if (stage == 0) {	// Send unit to despawn location
		int x, y;
		unit->getDespawnPoint(&x, &y);
		if (unit->voyage(x, y)) stage++;
		else wait();
	}
	else if (stage == 1) {	// Despawn on arrival
		if (unit->arrived)
			unit->destroy(), destroyed = true;
	}
}


void SeekBehaviour::init(Level *_l, Unit *_u) {
	level = _l, unit = _u;
	unit->capture();
	building = level->randomBuildingWithType(requisiteBuildingType());
	initialized = true;
}
void SeekBehaviour::_update() {
	if (stage == 0) {		// Send unit to requisite establishment
		int x, y;
		building->getEntryPoint(&x, &y);
		if (unit->voyage(x, y)) stage++;
		else wait();
	}
	else if (stage == 1) {	// Send unit to queue point
		if (unit->arrived) {
			int x, y;
			building->getQueuePoint(&x, &y);
			if (unit->voyage(x, y)) stage++;
			else wait();
		}
	}
	else if (stage == 2) {	// Done!
		if (unit->arrived) {
			unit->nextBehaviour = followingBehaviour();
			unit->release();
			building->addToQueue(unit);
			destroyed = true;
		}
	}
}


void ServiceBehaviour::init(Level *_l, Unit *_u, Unit *_s, Furnishing *_f) {
	level = _l, unit = _u, staff = _s, furnishing = _f;
	// Check furnishing is capable of this interaction
	if (!furnishing->capableOfBehaviour(typestring())) {
		char s[100]; sprintf(s, "'%s' behaviour initialized with incompatible '%s' furnishing", typestring(), furnishing->type.c_str());
		throw MsgException(s);
	}
	unit->capture(), staff->capture(), furnishing->capture();
	initialized = true;
}
void ServiceBehaviour::_update() {
	if (stage == 0) {		// Send units to interaction points
		int uDestX, uDestY, sDestX, sDestY;
		furnishing->getInteractionPoint("civilian", &uDestX, &uDestY);
		furnishing->getInteractionPoint("staff", &sDestX, &sDestY);
		if (uDestX < 0 || sDestY < 0) { }	// Incompatible unit! What to do? Hmm.
		if (unit->voyage(uDestX, uDestY) && staff->voyage(sDestX, sDestY)) stage++;
		else wait();
	}
	else if (stage == 1) {	// Run animations
		if (unit->arrived && staff->arrived)
			unit->runAnimation(), staff->runAnimation(), stage++;
	}
	else if (stage == 2) {	// Release things
		if (unit->animation_finished && staff->animation_finished) {
			level->createBehaviour("despawn")->init(unit);
			unit->release(), staff->release(), furnishing->release();
			destroyed = true;
		}
	}
}
