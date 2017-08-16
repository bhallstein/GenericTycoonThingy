/*
 * Building.h - a building
 *
 */

#ifndef BUILDING_H
#define BUILDING_H

#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "types.h"
#include "MappedObj.hpp"

class Building : public MappedObj
{
public:
	Building(); //for templates only!
	Building(int _x, int _y);
	~Building();

	// Methods
	void receiveEvent(Event *ev);	// Handle mouse events
	sf::Color col();						// Temporary coloration-signifier (now an actual colour!)

	// Properties
	sf::Color defaultCol; //read in from teh Lua
	std::string type;
	bool destroyed;
	std::vector<door> doors;		// Doors. These should probably be on the edge of the building. lol.
									// See types.h for what a door looks like.
protected:
	// Properties
	bool clicked;
};

#endif
