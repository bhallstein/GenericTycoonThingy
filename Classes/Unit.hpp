/* 
 * Unit.hpp
 *
 */

#ifndef UNIT_H
#define UNIT_H

#include <iostream>

#include "types.h"
#include "MappedObj.hpp"

class NavMap;
class NavNode;

class Unit : public MappedObj
{
public:
	Unit(NavMap *, int _x, int _y);
	~Unit();

	// Methods
	void receiveEvent(Event *);	// Override to handle events
	colour col();
	void update();

	// Properties
	int dest_x, dest_y;
	bool destroyed;
	
protected:
	// Methods
	void nextInRoute();
	void setToIdle();
	void setToTraveling();
	void setToWaiting();
	void incrementLocation();

	// Properties
	enum state_types { S_IDLE, S_TRAVELING, S_WAITING } state;
	int frames_waited;
	NavMap *navmap;
	std::vector<NavNode *> route;
	bool hover;

};

#endif
