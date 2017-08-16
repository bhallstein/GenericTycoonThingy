/*
 * Behaviour.hpp - a behaviour cooridnates units and furnishing in complex and beautiful ways
 *
 */

#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include <string>
#include <map>

#include "types.hpp"
#include "TLO.hpp"
#include "W.h"

#define WAIT_PERIOD 60

class Unit;
class Building;
class Furnishing;
class Level;
class LuaHelper;

class BehaviourBase;

class Behaviour {
public:
	Behaviour(const char *_type, W::EventHandler *);
	~Behaviour();
	static bool initialize();
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
	W::EventHandler *eh;
};

class BehaviourBase : public TLO {
public:
	BehaviourBase(W::EventHandler *_eh) : TLO(_eh), stage(0), waiting(false), initialized(false) { };
	virtual ~BehaviourBase() { }
	void update() {
		if (!initialized) throw W::Exception("update called on uninitialized Behaviour object");
		if (waiting && ++frames_waited >= WAIT_PERIOD)
			waiting = false;
		else
			update();
	}
	virtual void init(Unit *) { }
	virtual void init(Level *, Unit *) { }
	virtual void init(Unit *, Unit *, Furnishing *) { }
	virtual void init(Level *, Unit *, Unit *, Furnishing *) { }
	
	static bool initialize();
	
protected:
	int stage;
	int frames_waited;
	bool waiting;
	bool initialized;		// The init override should set this to true when done
	virtual void wait() {
		frames_waited = 0;
		waiting = true;
	}
	
	static LuaHelper *mrLua;
	static bool lua_initialized;
};

class DespawnBehaviour : public BehaviourBase {
public:
	DespawnBehaviour(W::EventHandler *_eh) : BehaviourBase(_eh) { }
	void init(Unit *);
	void update();
protected:
	Unit *unit;
};


/* SeekBehaviour: navigating a unit to a building of the required type */

struct seekBehaviourInfo {
	std::string requisiteBuildingType;
	std::string followingBehaviour;
};

class SeekBehaviour : public BehaviourBase {
public:
	SeekBehaviour(const char *_type, W::EventHandler *_eh);
	static bool initialize();
	void init(Level *, Unit *);
	void update();
protected:
	Level *level;
	Unit *unit;
	Building *building;
	
	static bool lua_initialized;
	static std::map<std::string, struct seekBehaviourInfo> types;
	std::string type;
	struct seekBehaviourInfo *bType;
};


/* ServiceBehaviour: a unit interacts with a placeable & a staff unit */

struct serviceBehaviourInfo {
	int charge;
};

class ServiceBehaviour : public BehaviourBase {
public:
	ServiceBehaviour(const char *_type, W::EventHandler *_eh);
	~ServiceBehaviour();
	static bool initialize();
	void init(Level *, Unit *u, Unit *s, Furnishing *f);
	void update();
	void receiveEvent(W::Event *);
protected:
	Level *level;
	Unit *unit, *staff;
	Furnishing *furnishing;
	
	Building *contextBuilding;	// Used in interruption
	
	static bool lua_initialized;
	static std::map<std::string, struct serviceBehaviourInfo> types;
	std::string type;
	struct serviceBehaviourInfo *bType;
};

#endif
