/*
 * PlaceableManager.hpp - base class providing placeability
 *
 */

#ifndef PlaceableManager_h
#define PlaceableManager_h

#include <vector>

#include "types.hpp"
#include "TLO.hpp"
#include "W.h"
#include "Placeable.hpp"

class PlaceableManager : public TLO {
public:
  PlaceableManager(LevelMap *, View__Game *, W::NavMap *, bool _placeableMode);
  bool init(W::v2f);  // Attempt to init at supplied position
  bool init(); // Attempt to init at current position

  bool pickUp();
  bool attemptToPlace(W::v2f);
  void cancel();

  // Fn overrides to implement placeability
  virtual void placementLoopStarted() = 0;
  virtual void placementLoopUpdate() = 0;
  virtual void placementLoopCancelled() = 0;
  virtual void placementLoopSucceeded() = 0;
  virtual bool canPlace(W::v2f) = 0;

  bool placeableMode;
  Placeable placeable;
};

#endif

