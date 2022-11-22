/*
 * Controller.hpp
 *
 * A Controller issues tasks to Units and Furnishings
 *
 */
 
#ifndef Controller_h
#define Controller_h

#include <map>

#include "TLO.hpp"
#include "UIDManager.hpp"

class Unit;

namespace ControllerCompletion {
  enum T { Failure, Success };
}

class Controller : public TLO {
public:
  Controller(LevelMap *, View__Game *, W::NavMap *);
  virtual ~Controller();
  void _setUp();

  bool dispatchUnit(Unit *);
    // If canDispatchUnitType( ) returns true, calls _dispatchUnit().
  bool dispatchUnit(Unit *, Controller *c_passback);
    // Does the same, but first adds the Unit to the pass-back map
    // (i.e. use if the Unit should be passed back to another Controller
    //  when finished with.)

  virtual void resume(Unit *, ControllerCompletion::T) = 0;
    // Called after being reactivated.

  virtual void success(Unit *) = 0;
  virtual void failure(Unit *) = 0;
    // These methods are called by Unit when it has successfully carried
    // out, or failed to sucessfully carry out, a task.

  static void initialize();

  virtual void unitPickedUp(Unit *) = 0;
  virtual void unitPutDown(Unit *) = 0;

protected:
  // Unit dispatch/pass-back
  virtual bool canDispatch(const std::string &) = 0;
  virtual void _dispatchUnit(Unit *) = 0;

  bool sendUnitToController(Unit *, Controller *);
    // Send a unit from this Controller to another: calls dispatchUnit()
    // on the foreign Controller, and if successful, makes this Controller
    // inactive.
  void releaseUnit(Unit *, ControllerCompletion::T = ControllerCompletion::Success);
    // If a previous Controller exists for the unit, reactivate it, then
    // call its resume() method.
    // Otherwise, destroy the Unit.

  virtual void getSDs(sdvec &vec) {
    TLO::getSDs(vec);
    vec.push_back(&sd);
  }

private:
  std::map<UID,UID> passbackMap;
    // After a Unit is done with, it is "passed back" to its previous
    // Controller, if it had one, which is saved in this map.

  static serialization_descriptor sd;
};


/* CustomerController */

class CustomerController : public Controller {
public:
  CustomerController(LevelMap *, View__Game *, W::NavMap *, SeekTarget::Type);

  void resume(Unit *, ControllerCompletion::T);
  void success(Unit *) { ++stage; }
  void failure(Unit *) { stage = failureStage; }

  void update();

  static void initialize();

  void unitPickedUp(Unit *);
  void unitPutDown(Unit *);

protected:
  bool canDispatch(const std::string &);
  void _dispatchUnit(Unit *);

  int stage;
  int failureStage;
  int timeWaited;
  UID customer;
  UID dest_building;
  SeekTarget::Type seek_target;

  virtual void getSDs(sdvec &vec) {
    Controller::getSDs(vec);
    vec.push_back(&sd);
  }

private:
  Unit* customerPtr() { return (Unit *) customer.get(); }
  static serialization_descriptor sd;
};


/* ShopkeeperController */

namespace W {
  namespace EventType {
    extern int Haircut;
    extern int PieSale;
  }
}

class ShopkeeperController : public Controller {
public:
  ShopkeeperController(LevelMap *, View__Game *, W::NavMap *);

  void resume(Unit *, ControllerCompletion::T);
  void success(Unit *) { ++stage; }
  void failure(Unit *) { stage = failureStage; }

  void update();

  static void initialize();

  void unitPickedUp(Unit *);
  void unitPutDown(Unit *);

//  bool dispatchCustomer(Unit *);

protected:
  bool canDispatch(const std::string &);
  void _dispatchUnit(Unit *);

  int stage;
  int failureStage;
  int timeWaited;
  UID shopkeeper;
  UID building;
  UID customer;
  UID furnishing;

  virtual void getSDs(sdvec &vec) {
    Controller::getSDs(vec);
    vec.push_back(&sd);
  }

private:
  static serialization_descriptor sd;
};

#endif

