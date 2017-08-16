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

#include "EventResponder.h"

 
class Unit : EventResponder
{
public:
	Unit();
	~Unit();

	/* Methods */

	// Override receive event method
	void receiveEvent(sf::Event *ev, EventResponder **p_e_r);


protected:


};

#endif