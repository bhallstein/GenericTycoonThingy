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
#include "GameMap.hpp"


class Unit : public EventResponder
{
public:
	Unit(GameMap *map);
	~Unit();

	// Methods
	void receiveEvent(sf::Event *ev, EventResponder **p_e_r);	// Override to handle events
	char col();

	// Properties
	int a, b;		// Floating point offset from block
	
	std::string state;

protected:

	// Properties
	GameMap *gamemap;
	
};

#endif
