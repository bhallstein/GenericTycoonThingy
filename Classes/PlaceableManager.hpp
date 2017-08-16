/*
 * PlaceableManager.hpp - 
 *
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
	PlaceableManager( bool _placeableMode);
	~PlaceableManager();
	bool init(int _x, int _y);
	
	bool pickUp();
	bool attemptToPlace(int _x, int _y);
	virtual void finalizePlacement() { };	// Override, for instance to add to an eventhandler or navmap
	virtual bool canPlace(int _x, int _y) = 0;
	
	bool placeableMode;
	Placeable *placeable;
};

#endif
