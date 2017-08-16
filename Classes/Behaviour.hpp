/*
 * Behaviour.hpp - a behaviour cooridnates units and furnishing in complex and beautiful ways
 *
 */

#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include <string>

#define WAIT_PERIOD 60

class Unit;
class Building;
class Furnishing;
class Level;

class BehaviourBase;

class Behaviour {
public:
	Behaviour(const char *_type);
	~Behaviour();
	void update();
	bool destroyed();
	void destroy();
	void init(Unit *u);
	void init(Level *, Unit *);
	void init(Unit *, Unit *, Furnishing *);
	void init(Level *l, Unit *, Unit *, Furnishing *);
protected:
	BehaviourBase *b;
	std::string type;
};

class BehaviourBase {
public:
	BehaviourBase() : stage(0), destroyed(false), waiting(false) { };
	void update() {
		if (waiting && ++frames_waited >= WAIT_PERIOD)
			waiting = false;
		else
			_update();
	}
	virtual void _update() = 0;
	virtual void init(Unit *) { }
	virtual void init(Level *, Unit *) { }
	virtual void init(Unit *, Unit *, Furnishing *) { }
	virtual void init(Level *, Unit *, Unit *, Furnishing *) { }
	bool destroyed;
protected:
	int stage;
	int frames_waited;
	bool waiting;
	virtual void wait() {
		frames_waited = 0;
		waiting = true;
	}
};

class DespawnBehaviour : public BehaviourBase {
public:
	void init(Unit *);
	void _update();
protected:
	Unit *unit;
};

class SeekHaircutBehaviour : public BehaviourBase {
public:
	void init(Level *, Unit *);
	void _update();
protected:
	Level *level;
	Unit *unit;
	Building *barbershop;
};

class HaveHaircutBehaviour : public BehaviourBase {
public:
	void init(Level *, Unit *u, Unit *s, Furnishing *f);
	void _update();
protected:
	Level *level;
	Unit *unit, *staff;
	Furnishing *chair;
	std::string typestring;
};

#endif
