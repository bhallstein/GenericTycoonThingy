/*
 * Generic Tycoon Thingy
 *
 * ==================
 *  Controller.cpp
 * ==================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#include "Controller.hpp"
#include "Unit.hpp"
#include "UIDManager.hpp"
#include "LevelMap.hpp"

/********************/
/* Controller impl. */
/********************/

Serializable::serialization_descriptor Controller::sd;

Controller::Controller(LevelState *_ls, LevelMap *_lm, LevelView *_lv, W::NavMap *_nm) :
	TLO(_ls, _lm, _lv, _nm)
{
	// Hai mr controlley
}
Controller::~Controller()
{
	// Bye now
}
void Controller::_setUp() {
	
}

bool Controller::dispatchUnit(Unit *u) {
	if (!canDispatch(u->type)) return false;
	u->controller = uid;
	_dispatchUnit(u);
	return true;
}
bool Controller::dispatchUnit(Unit *u, Controller *c) {
	if (!canDispatch(u->type)) return false;
	passbackMap[u->uid] = c->uid;
	u->controller = uid;
	_dispatchUnit(u);
	return true;
}

void Controller::releaseUnit(Unit *u, ControllerCompletion::T succ) {
	std::map<UID,UID>::iterator it = passbackMap.find(u->uid);
	if (it == passbackMap.end()) {
		u->destroy();
	}
	else {
		Controller *c = (Controller*) it->second.get();
		levelMap->reactivateController(c);
		c->resume(u, succ);
	}
}

bool Controller::sendUnitToController(Unit *u, Controller *c) {
	bool dispatched = c->dispatchUnit(u);
	if (dispatched) levelMap->deactivateController(this);
	return dispatched;
}


/****************************/
/* CustomerController impl. */
/****************************/

Serializable::serialization_descriptor CustomerController::sd;

CustomerController::CustomerController(LevelState *_ls, LevelMap *_lm, LevelView *_lv, W::NavMap *_nm) :
	Controller(_ls, _lm, _lv, _nm),
	stage(0),
	failureStage(0)
{
	type = "CustomerController";
}

void CustomerController::resume(Unit *_u, ControllerCompletion::T succ) {
	// if success, do things, otherwise, do other things
}
void CustomerController::update() {
	/* No unit dispatched */
	if (stage == 0) {
		
	}
	
	/* Wander between random locations */
	else if (stage == 1) {
		failureStage = 100;
		customerPtr()->wanderToRandomMapDestination();
		++stage;
	}
	else if (stage == 2) {
		// wait for unit to arrive at destination
	}
	else if (stage == 3) {
		// pause for ~2s
		if (++timeWaited >= 120) {
			timeWaited = 0;
			stage = 1;
		}
	}
	
	/* Failure: wander to a random destination */
	else if (stage == 100) {
		stage = 3;	// Wait 2s then try again
	}
	
	/* Unit has been picked up */
	else if (stage == 500) {
		
	}
}

bool CustomerController::canDispatch(const std::string &type) {
	return (customer.isEmpty() && type == "customer");
}
void CustomerController::_dispatchUnit(Unit *u) {
	customer = u->uid;
	if (u->placeableMode) stage = 500;
	else stage = 1;
}

void CustomerController::unitPickedUp(Unit *u) {
	stage = 500;
}
void CustomerController::unitPutDown(Unit *u) {
	stage = 1;
	// May want to check context, do things here.
}
