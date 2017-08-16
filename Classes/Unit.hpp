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


class Unit : public EventResponder
{
public:
	Unit(NavMap *);
	~Unit();

	// Methods
	void receiveEvent(sf::Event *ev, EventResponder **p_e_r);	// Override to handle events
	char col();
	void goSomewhere(int x, int y);

	// Properties
	int a, b;		// Floating point offset from block
	int destX, destY;
	
	bool destroyed;
	
	std::string state;


protected:

	// Properties
	NavMap *navmap;
	
};

#endif
