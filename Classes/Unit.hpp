/* 
 * Unit.h
 *
 */

#ifndef UNIT_H
#define UNIT_H

#include <iostream>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "types.h"
#include "MappedObj.hpp"
#include "NavMap.hpp"


class Unit : public MappedObj
{
public:
	Unit(NavMap *, int _x, int _y);
	~Unit();

	// Methods
	void receiveEvent(Event *);	// Override to handle events
	char col();
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
	std::vector<NavNode*> route;

};

#endif
