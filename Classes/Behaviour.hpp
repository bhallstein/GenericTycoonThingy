/*
 * Behaviour.hpp - a behaviour cooridnates units and furnishing in complex and beautiful ways
 *
 */

#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include <string>
#include <map>

#include "types.hpp"
#include "EventResponder.hpp"

#define WAIT_PERIOD 60

class Unit;
class Building;
class Furnishing;
class Level;
class ResponderMap;
class W;
class LuaHelper;

class BehaviourBase;

class Behaviour {
public:
	Behaviour(const char *_type, ResponderMap *_levelRM);
	~Behaviour();
	static bool initialize(W *);
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

class BehaviourBase : public EventResponder {
public:
	BehaviourBase(ResponderMap *_levelRM) : levelRM(_levelRM), stage(0), destroyed(false), waiting(false), initialized(false) { };
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
	virtual void receiveEvent(Event *) { }

	static bool initialize(W *);
	
protected:
	int stage;
	int frames_waited;
	bool waiting;
	bool initialized;		// The init override should set this to true when done
	virtual void wait() {
		frames_waited = 0;
		waiting = true;
	}
	ResponderMap *levelRM;
	
	static LuaHelper *mrLua;
	static bool lua_initialized;
};

class DespawnBehaviour : public BehaviourBase {
public:
	DespawnBehaviour(ResponderMap *rm) : BehaviourBase(rm) { }
	void init(Unit *);
	void _update();
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
	SeekBehaviour(const char *_type, ResponderMap *rm);
	static bool initialize(W *);
	void init(Level *, Unit *);
	void _update();
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
	ServiceBehaviour(const char *_type, ResponderMap *rm);
	~ServiceBehaviour();
	static bool initialize(W *);
	void init(Level *, Unit *u, Unit *s, Furnishing *f);
	void _update();
	void receiveEvent(Event *);
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
