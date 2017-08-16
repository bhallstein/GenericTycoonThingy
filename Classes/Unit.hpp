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

#define S_IDLE 1
#define S_TRAVELING 2

class Unit : public EventResponder
{
public:
	Unit(NavMap *, int _x, int _y);
	~Unit();

	// Methods
	void receiveEvent(sf::Event *ev, EventResponder **p_e_r);	// Override to handle events
	char col();
	void goSomewhere(int x, int y);
	void update();

	// Properties
	float a, b;		// Floating point offset from block
	int destX, destY;
	
	bool destroyed;
	
	int state;


protected:
	// Methods
	void nextInRoute();

	// Properties
	NavMap *navmap;
	std::vector<NavNode*> route;
	
};

#endif
