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

class Behaviour : public TLO {
public:
	Behaviour();
	virtual ~Behaviour();
	void update() {
		// This waiting routine is provided so that subclasses can simply
		// call wait() to wait a fixed amount of time
		if (waiting) {
			if (++frames_waited >= WAIT_PERIOD)
				waiting = false;
		}
		else
			_update();
	}
	virtual void _update() = 0;
	static bool initialize();
	void destroy() { destroyed = true; }
	
protected:
	int stage;				// Stages always start at zero
	int frames_waited;
	bool waiting;
	void wait() {
		frames_waited = 0;
		waiting = true;
	}
	
	static LuaHelper *mrLua;
	static bool lua_initialized;
};


/*
	SuperBehaviours implement broader tasks such as "being a customer",
	work by chaining behaviours together.
*/

class SuperBehaviour /* to the rescue */ : public Behaviour {
public:
	SuperBehaviour();
	virtual ~SuperBehaviour();
	void _update();
	virtual void _superupdate() = 0;	// Implementation for SBs goes in this override
	void unsuspend();
protected:
	Behaviour *yield_behaviour;		// A yield behaviour is one created by the SB itself
	void yield(Behaviour *);
	void unyield();				// Deletes yield_behaviour & resumes updating the SB
	bool suspended;			// When a SB instead dispatches a unit U to an externally owned
	void suspend();			// dispatching behaviour E, it should then call suspend. E is responsible
							// for calling unsuspend when it has finished with U.
};

/*
	Yielding
	- When a SB creates another behaviour, it yields to it.
	- After yield is called, the SB will call the yield behaviour’s update method indstead of its own.
	- When the yield behaviour sets itself to destroyed with destroy(), the SB will call unyield.
	  This deletes yield_behaviour and resumes updating the SB.
 
	Suspending
	- When a SB dispatches a unit U to an externally owned dispatching behaviour B, it should then call suspend.
	- E is responsible for calling unsuspend when it has finished with U.
 
	NOTE
	- If the SB has captured participants, they should be creating a new behaviour or dispatching a unit to a dispatching behaviour.
*/


class CustomerBehaviour : public SuperBehaviour {
public:
	CustomerBehaviour(Unit *, Level *);
	~CustomerBehaviour();
	void _superupdate();
	
	void buildingAccessGranted();	// Called by Building when dequeueing a unit from its queue
protected:
	Unit *customer;
	Level *level;
	Building *destination_building;
	Furnishing *itemForPurchase;
};


/*
	"Normal" behaviours
*/

class RouteBehaviour : public Behaviour {
public:
	RouteBehaviour(Unit *, W::position &_dest);
	~RouteBehaviour();
	void _update();
private:
	Unit *unit;
	W::position dest;
};


/*
	Dispatching Behaviours
*/

class DispatchingBehaviour : public SuperBehaviour {
public:
	DispatchingBehaviour();
	virtual bool dispatchUnit(Unit *, SuperBehaviour *) { return false; }
	virtual bool dispatchUnit(Unit *, Furnishing *, SuperBehaviour *) { return false; }
	virtual void finishedDispatch() = 0;	// When DB has finished performing task, should unsuspend its calling SB
	bool isAvailable();
protected:
	bool available;
	// Relating to particular dispatch instance
	SuperBehaviour *sb;
};

class ShopKeeperBehaviour : public DispatchingBehaviour {
public:
	ShopKeeperBehaviour(Unit *, Level *);
	~ShopKeeperBehaviour();
	void _superupdate();
	bool dispatchUnit(Unit *, Furnishing *, SuperBehaviour *);
	void finishedDispatch();
	
	void unitWasPickedUp();
	void unitWasPutDown();
	
protected:
	Unit *sk;
	Building *contextBuilding;
	Level *level;

	// Relating to particular dispatch instance
	Unit *customer;
	Furnishing *furnishing;
};

class PurchaseBehaviour : public Behaviour {
public:
	PurchaseBehaviour(Unit *_customer, Unit *_staff, Furnishing *, Level *);
	void _update();
private:
	Unit *customer;
	Unit *staff;
	Furnishing *furnishing;
	Level *level;
};

//class QueueBehaviour : public DispatchingBehaviour {
//public:
//	QueueBehaviour(W::EventHandler *, Building *, int _qsize);
//	~QueueBehaviour();
//	void _update();
//	bool dispatchUnit(Unit *, SuperBehaviour *);	// NB: dispatch *to* the queue, not from it!
//private:
//	std::queue<Unit*> Q;
//	bool queue(Unit *, SuperBehaviour *);
//	void dequeue();
//	std::map<Unit*, SuperBehaviour*> sbs;
//	
//	Building *shop;
//	int qsize;
//};


/* ServiceBehaviour: a unit interacts with a placeable & a staff unit */

//struct serviceBehaviourInfo {
//	int charge;
//};
//
//class ServiceBehaviour : public Behaviour {
//public:
//	ServiceBehaviour(W::EventHandler *_eh);
//	~ServiceBehaviour();
//	static bool initialize();
//	void init(Level *, Unit *u, Unit *s, Furnishing *f);
//	void _update();
//	void receiveEvent(W::Event *);
//protected:
//	Level *level;
//	Unit *unit, *staff;
//	Furnishing *furnishing;
//	
//	Building *contextBuilding;	// Used in interruption
//	
//	static bool lua_initialized;
//	static std::map<std::string, struct serviceBehaviourInfo> types;
//	std::string type;
//	struct serviceBehaviourInfo *bType;
//};

#endif
