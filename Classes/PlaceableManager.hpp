/*
 * PlaceableManager.hpp - base class providing placeability
 *
 */

#ifndef PlaceableManager_H
#define PlaceableManager_H

#include <vector>

#include "types.hpp"
#include "TLO.hpp"
#include "W.h"
#include "Placeable.hpp"

class PlaceableManager : public TLO {
public:
	PlaceableManager(LevelState *, LevelMap *, LevelView *, W::NavMap *, bool _placeableMode);
	bool init(const W::position &);	// Attempt to init at supplied position
	bool init(); // Attempt to init at current position
	
	bool pickUp();
	bool attemptToPlace(const W::position &);
	void cancel();
	
	// Fn overrides to implement placeability
	virtual void placementLoopStarted() = 0;
	virtual void placementLoopUpdate() = 0;
	virtual void placementLoopCancelled() = 0;
	virtual void placementLoopSucceeded() = 0;
	virtual bool canPlace(const W::position &) = 0;
	
	bool placeableMode;
	Placeable placeable;
};

#endif
