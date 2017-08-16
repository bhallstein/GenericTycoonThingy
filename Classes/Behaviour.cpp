#include "Behaviour.hpp"
#include "Unit.hpp"
#include "Building.hpp"
#include "Level.hpp"
#include "Furnishing.hpp"

Behaviour::Behaviour(const char *_type) : type(_type) 
{
	if      (strcmp(_type, "seekhaircut") == 0) b = new SeekHaircutBehaviour();
	else if (strcmp(_type, "havehaircut") == 0) b = new HaveHaircutBehaviour();
	else if (strcmp(_type, "despawn") == 0)     b = new DespawnBehaviour();
	else throw MsgException("Behaviour with invalid type created");
}
Behaviour::~Behaviour()
{
	std::cout << "Behaviour \"" << type << "\" destruct" << std::endl;
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
			unit->despawn(), destroyed = true;
	}
}

void SeekHaircutBehaviour::init(Level *_level, Unit *_u) {
	unit = _u;
	level = _level;
	barbershop = level->randomBuildingWithType("barber");
}
void SeekHaircutBehaviour::_update() {
	if (stage == 0) {		// Send customer to barber shop
		int x, y;
		barbershop->getEntryPoint(&x, &y);
		if (unit->voyage(x, y)) stage++;
		else wait();
	}
	else if (stage == 1) {	// Send customer to queue point
		if (unit->arrived) {
			int x, y;
			barbershop->getQueuePoint(&x, &y);
			if (unit->voyage(x, y)) stage++;
			else wait();
		}
	}
	else if (stage == 2) {	// Done!
		if (unit->arrived) {
			barbershop->addToQueue(unit);
			destroyed = true;
		}
	}
}

void HaveHaircutBehaviour::init(Level *_l, Unit *_u, Unit *_s, Furnishing *_f) {
	level = _l;
	unit = _u;
	staff = _s;
	chair = _f;
	typestring = "havehaircut";
	// Check chair is capable of this interaction
	if (!chair->capableOfInteraction(typestring.c_str())) {
		char s[100]; sprintf(s, "HaveHaircutBehaviour initialized with incompatible '%s' furnishing", chair->type.c_str());
		throw MsgException(s);
	}
	chair->capture();
}
void HaveHaircutBehaviour::_update() {
	if (stage == 0) {		// Send units to interaction points
		int uDestX, uDestY, sDestX, sDestY;
		chair->getInteractionPoint("civilian", &uDestX, &uDestY);
		chair->getInteractionPoint("staff", &sDestX, &sDestY);
		if (uDestX < 0 || sDestY < 0) { }	// Incompatible unit! What to do? Hmm.
		if (unit->voyage(uDestX, uDestY) && staff->voyage(sDestX, sDestY)) stage++;
		else wait();
	}
	else if (stage == 1) {	// Run animations
		if (unit->arrived && staff->arrived)
			chair->release(), stage++;
	}
	else if (stage == 2) {	// Release Unit
		level->createBehaviour("despawn")->init(unit);
		destroyed = true;
	}
}
