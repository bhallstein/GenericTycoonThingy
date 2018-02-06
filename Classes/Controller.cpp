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
#include "Building.hpp"
#include "Serializer.hpp"
#include "Messenger.h"

/********************/
/* Controller impl. */
/********************/

Serializable::serialization_descriptor Controller::sd;

Controller::Controller(LevelMap *_lm, LevelView *_lv, W::NavMap *_nm) :
	TLO(_lm, _lv, _nm)
{
	// Hai mr controlley
}
Controller::~Controller()
{
	std::cout << "controller " << this << " destruct" << std::endl;
}
void Controller::initialize() {
	sd["passbackMap"] = makeSerializer(&Controller::passbackMap);
}
void Controller::_setUp() {
	
}



// Public:
bool Controller::dispatchUnit(Unit *u) {
	if (!canDispatch(u->type)) {
		return false;
	}
	u->controller = uid;
	_dispatchUnit(u);
	return true;
}
bool Controller::dispatchUnit(Unit *u, Controller *c_passback) {
	if (!canDispatch(u->type)) {
		return false;
	}
	passbackMap[u->uid] = c_passback->uid;
	u->controller = uid;
	_dispatchUnit(u);
	return true;
}

// Protected:
bool Controller::sendUnitToController(Unit *u, Controller *c) {
	bool dispatched = c->dispatchUnit(u, this);
	if (dispatched) {
		levelMap->deactivateController(this);
	}
	return dispatched;
}

void Controller::releaseUnit(Unit *u, ControllerCompletion::T succ) {
	auto it = passbackMap.find(u->uid);
	if (it == passbackMap.end()) {
		u->destroy();
	}
	else {
		Controller *c = (Controller*) it->second.get();
		levelMap->reactivateController(c);
		u->controller = c->uid;
		c->resume(u, succ);
	}
}


/****************************/
/* CustomerController impl. */
/****************************/

Serializable::serialization_descriptor CustomerController::sd;

CustomerController::CustomerController(LevelMap *_lm, LevelView *_lv, W::NavMap *_nm, SeekTarget::Type _seek) :
	Controller(_lm, _lv, _nm),
	stage(0),
	failureStage(0),
	seek_target(_seek)
{
	type = "CustomerController";
}
void CustomerController::initialize() {
	sd["stage"] = makeSerializer(&CustomerController::stage);
	sd["failureStage"] = makeSerializer(&CustomerController::failureStage);
	sd["timeWaited"] = makeSerializer(&CustomerController::timeWaited);
	sd["customer"] = makeSerializer(&CustomerController::customer);
	sd["seek_target"] = makeSerializer(&CustomerController::seek_target);
}

void CustomerController::resume(Unit *_u, ControllerCompletion::T succ) {
	if (succ == ControllerCompletion::Success) {
		++stage;
	}
	else {
		stage = failureStage;
	}
}
void CustomerController::update() {
	/* No unit dispatched */
	if (stage == 0) {
		
	}
	
	
	/* Wander to an available building or between random locations */
	else if (stage == 1) {
		failureStage = 99;
		
		// Try to find a building supporting the seek target
		Building *b = levelMap->building__withFurnishingSupportingSeekTarget(seek_target);
		if (b) {
			dest_building = b->uid;
			stage = 100;
		}
		else {
			bool route_found = customerPtr()->wanderToRandomMapDestination();
			if (route_found) {
				++stage;
			}
			else {
				stage = failureStage;
			}
		}
	}
	else if (stage == 2) {
		// wait for unit to arrive at destination
	}
	else if (stage == 3) {
		// pause for ~2s
		if (++timeWaited >= 1) {
			timeWaited = 0;
			stage = 1;
		}
	}
	else if (stage == 99) {
		// Failure
		stage = 3;	// Wait 2s then try again
	}
	
	
	/* Navigate to dest building */
	else if (stage == 100) {
		failureStage = 199;
		Building *b = (Building*) dest_building.get();
		
		customerPtr()->voyage(b->rct.pos - W::position(1, 0));
		++stage;
	}
	else if (stage == 101) {
		// wait for unit to arrive at shop entry point
	}
	else if (stage == 102) {
		// wait for a shopkeeper to be available
		Building *b = (Building*) dest_building.get();
		auto sks = b->shopkeeper__getAll();
		
		for (auto it = sks.begin(); it != sks.end(); ++it) {
			ShopkeeperController *sk = (ShopkeeperController*) (*it).get();
			if (sendUnitToController(customerPtr(), sk)) {
				++stage;
				break;
			}
		}
	}
	else if (stage == 103) {
		// wait for shopkeeper to be done with customer
	}
	else if (stage == 104) {
		stage = 200;
	}
	else if (stage == 199) {
		// Failure
		stage = 1;
	}
	
	
	/* Send unit home */
	else if (stage == 200) {
		failureStage = 299;
		customerPtr()->wanderToRandomMapDestination();
		++stage;
	}
	else if (stage == 201) {
		
	}
	else if (stage == 202) {
		customerPtr()->destroy();
		this->destroy();
	}
	else if (stage == 299) {
		// Failure
		stage = 200;
	}
	
	
	/* Unit has been picked up */
	else if (stage == 500) {
		
	}
	
	
	else {
		throw new W::Exception(std::string("CustomerController '") + type + "' entered unexpected stage");
	}
}

bool CustomerController::canDispatch(const std::string &type) {
	return (customer.isEmpty() && type == "customer");
}
void CustomerController::_dispatchUnit(Unit *u) {
	customer = u->uid;
	stage = u->placeableMode ? 500 : 1;
}

void CustomerController::unitPickedUp(Unit *u) {
	stage = 500;
}
void CustomerController::unitPutDown(Unit *u) {
	stage = 1;
	// May want to check context, do things here.
}


/****************************/
/* ShopkeeperController impl. */
/****************************/

Serializable::serialization_descriptor ShopkeeperController::sd;

ShopkeeperController::ShopkeeperController(LevelMap *_lm, LevelView *_lv, W::NavMap *_nm) :
	Controller(_lm, _lv, _nm),
	stage(0),
	failureStage(0)
{
	type = "ShopkeeperController";
}
void ShopkeeperController::initialize() {
	sd["stage"] = makeSerializer(&ShopkeeperController::stage);
	sd["failureStage"] = makeSerializer(&ShopkeeperController::failureStage);
	sd["timeWaited"] = makeSerializer(&ShopkeeperController::timeWaited);
	sd["shopkeeper"] = makeSerializer(&ShopkeeperController::shopkeeper);
}


void ShopkeeperController::resume(Unit *_u, ControllerCompletion::T succ) {
	// if success, do things, otherwise, do other things
}
void ShopkeeperController::update() {
	/* No unit dispatched */
	if (stage == 0) {
		
	}
	
	
	/* Wander between random locations */
	else if (stage == 1) {
		failureStage = 99;
		Unit *sk = (Unit*) shopkeeper.get();
		bool route_found = sk->wanderToRandomMapDestination();
		if (route_found) {
			timeWaited = 0;
			++stage;
		}
		else {
			stage = failureStage;
		}
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
	else if (stage == 99) {
		// Failure
		stage = 3;	// Wait 2s then try again
	}
	
	
	/* Shop-keeping */
	else if (stage == 200) {
		// Wait for customer
		failureStage = 299;
	}
	else if (stage == 201) {
		// Send customer & shopkeeper to their destinations
		Unit *sk = (Unit*) shopkeeper.get();
		Unit *cust = (Unit*) customer.get();
		Building *b = (Building*) building.get();
		
		sk->voyage(b->rct.pos);
		cust->voyage(b->rct.pos + W::position(1, 0));
		
		timeWaited = 0;
		++stage;
	}
	else if (stage == 202) {
		// Wait for unit 1
	}
	else if (stage == 203) {
		// Wait for unit 2
	}
	else if (stage == 204) {
		// Do the interaction
		if (++timeWaited >= 60) {
			releaseUnit((Unit*) customer.get());
			++stage;
		}
	}
	else if (stage == 205) {
		// Interaction finished: wait for next customer
		stage = 200;
	}
	else if (stage == 299) {
		// Failure: one of the units has failed to reach the interaction destination
		Unit *cust = (Unit*) customer.get();
		releaseUnit(cust);
		stage = 200;
	}
	
	
	/* Unit has been picked up */
	else if (stage == 500) {
		
	}
	
	
	else {
		throw new W::Exception(std::string("ShopkeeperController '") + type + "' entered unexpected stage");
	}
}

bool ShopkeeperController::canDispatch(const std::string &type) {
	if (type == "shopkeeper") {
		return shopkeeper.isEmpty();
	}
	if (type == "customer") {
		return stage == 200;
	}
	return false;
}
void ShopkeeperController::_dispatchUnit(Unit *u) {
	if (u->type == "shopkeeper") {
		shopkeeper = u->uid;
		stage = u->placeableMode ? 500 : 1;
	}
	else if (u->type == "customer") {
		customer = u->uid;
		stage = 201;
	}
}

void ShopkeeperController::unitPickedUp(Unit *u) {
	stage = 500;
	
	// Remove from any current building
	Building *b = levelMap->building__findAt(u->rct.pos);
	if (b) {
		b->removeShopkeeper(uid);
	}
}
void ShopkeeperController::unitPutDown(Unit *u) {
	stage = 1;

	// If in a building, act accordingly
	Building *b = levelMap->building__findAt(u->rct.pos);
	if (b) {
		building = b->uid;
		b->addShopkeeper(uid);
		stage = 200;
	}
	else {
		building = NULL;
	}
}

