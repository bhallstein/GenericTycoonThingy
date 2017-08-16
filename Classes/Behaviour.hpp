/*
 * Behaviour.hpp - a behaviour cooridnates units and furnishing in complex and beautiful ways
 *
 */

#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include <string>
#include <map>
#include <queue>

#include "types.hpp"
#include "TLO.hpp"
#include "W.h"

#define WAIT_PERIOD 60

class Unit;
class Building;
class Furnishing;
class Level;
class LuaHelper;


class Behaviour : public Serializable {
public:
	Behaviour(Unit *_u) : u(_u), stage(0), failureStage(0) { }
	virtual ~Behaviour() { }
	
	// Unit::update() first updates itself, then calls Bhvr::update()
	virtual void update() = 0;

	// When a Unit completes a task, it calls success(), which increments
	// the bhvr’s stage property
	virtual void success() { ++stage; }

	// If a Unit cannot complete a task, it calls failure(), which causes
	// the bhvr to jump to the current failure stage
	virtual void failure() { stage = failureStage; }
	
	static bool initialize() {
		sd["stage"] = makeSerializer(&Behaviour::stage);
		sd["failure_stage"] = makeSerializer(&Behaviour::failureStage);
		return true;
	}
	
protected:
	Unit *u;
	int stage;
	int failureStage;
	
	sdvec getSDs() {
		sdvec vec, vec2 = _getSDs();
		vec.push_back(&Behaviour::sd);
		vec.insert(vec.end(), vec2.begin(), vec2.end());
		return vec;
	}
	virtual sdvec _getSDs() = 0;
	
private:
	static serialization_descriptor sd;
};


class CustomerBehaviour : public Behaviour {
public:
	CustomerBehaviour(Unit *_u) : Behaviour(_u), timeWaited(0) { }
	
	void update();
	static bool initialize() {
		sd["timeWaited"] = makeSerializer(&CustomerBehaviour::timeWaited);
		return true;
	}
	
protected:
	int timeWaited;
	
	sdvec _getSDs() {
		sdvec vec;
		vec.push_back(&CustomerBehaviour::sd);
		return vec;
	}
	
private:
	static serialization_descriptor sd;
};

#endif
