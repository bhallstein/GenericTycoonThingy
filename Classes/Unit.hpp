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

#include "EventResponder.hpp"
#include "NavMap.hpp"

#define S_IDLE      1		// Doing nothing
#define S_TRAVELING 2		// Traveling along a route
#define S_WAITING   3		// Periodically retrying finding a path to destination

class Unit : public EventResponder
{
public:
	Unit(NavMap *, int _x, int _y);
	~Unit();

	// Methods
	void receiveEvent(sf::Event *ev, EventResponder **p_e_r);	// Override to handle events
	char col();
	void update();

	// Properties
	float a, b;		// Floating point offset from block
	int dest_x, dest_y;
	
	bool destroyed;
	
	int state;


protected:
	// Methods
	void nextInRoute();
	void setToIdle();
	void setToTraveling();
	void setToWaiting();
	void incrementLocation();

	// Properties
	NavMap *navmap;
	std::vector<NavNode*> route;
	int frames_waited;
};

#endif
