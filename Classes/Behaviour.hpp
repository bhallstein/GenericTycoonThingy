/*
 * Behaviour.hpp - a behaviour cooridnates units and furnishing in complex and beautiful ways
 *
 */

#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include <string>

#include "types.hpp"

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
	BehaviourBase() : stage(0), destroyed(false), waiting(false), initialized(false) { };
	virtual ~BehaviourBase() { }
	void update() {
		if (!initialized) throw MsgException("update called on uninitialized Behaviour object");
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
	bool initialized;		// The init override should set this to true when done
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


/* Seek behaviours: navigating a unit to a building of the required type */

class SeekBehaviour : public BehaviourBase {
public:
	void init(Level *, Unit *);
	void _update();
protected:
	Level *level;
	Unit *unit;
	Building *building;
	virtual const char * requisiteBuildingType() = 0;
	virtual const char * followingBehaviour() = 0;
};

class SeekHaircutBehaviour : public SeekBehaviour {
protected:
	const char * requisiteBuildingType() { return "barber"; }
	const char * followingBehaviour() { return "havehaircut"; }
};
class SeekPieBehevaiour : public SeekBehaviour {
protected:
	const char * requisiteBuildingType() { return "pieshop"; }
	const char * followingBehaviour() { return "piesale"; }
};


/* Service behaviours: a unit interacts with a placeable & a staff unit */

class ServiceBehaviour : public BehaviourBase {
public:
	void init(Level *, Unit *u, Unit *s, Furnishing *f);
	void _update();
protected:
	virtual int serviceCharge() = 0; //the charge for this "service"
	Level *level;
	Unit *unit, *staff;
	Furnishing *furnishing;
	virtual const char * typestring() = 0;
};

class HaveHaircutBehaviour : public ServiceBehaviour {
protected:
	const char * typestring() { return "havehaircut"; }
	int serviceCharge() { return 1; }
};
class PieSaleBehaviour : public ServiceBehaviour {
protected:
	const char * typestring() { return "piesale"; }
	int serviceCharge() { return 3; }
};

#endif
