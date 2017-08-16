/*
 * PlaceableManager.hpp
 *
 * Implementing Placeability is done by overriding PM's virtual methods, as follows:
 *
 * 1. We enter the placement loop
 *     - This may be at construct time, or when pickUp() is called.
 *     - placementLoopStarted() is called
 *			- update the drawn object to its placement loop drawing state
 * 2. The Placeable is moved around
 *		- placementLoopUpdate() is called
 *			- update the drawn object's position
 *			- do not update the subclass's own position, only the drawn object’s, as the new
 *			  position is not yet established
 * 3a Placement is attempted in a new location
 *		- canPlace(pos) is called
 *			- return whether the object can be placed at the given location
 *		- if canPlace returned true
 *			- the object’s new position is set
 * 3b Placement is cancelled
 * 4. The placement loop ends
 *		- if cancelled, placementLoopCancelled() called
 *			- may want to set the object to destroyed, or put it back where it was
 *		- otherwise, placementLoopSucceeded() called
 *			- set the drawn object's properties from the object's new position
 *			  and put it back in its non-placement loop drawing state
 */

#ifndef PlaceableManager_H
#define PlaceableManager_H

#include <vector>

#include "types.hpp"
#include "TLO.hpp"
#include "W.h"

class Placeable;

class PlaceableManager : public TLO {
public:
	PlaceableManager(LevelState *, LevelMap *, LevelView *, W::NavMap *, bool _placeableMode);
	~PlaceableManager();
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
	Placeable *placeable;
};

#endif
