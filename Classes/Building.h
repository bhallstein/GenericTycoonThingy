/*
 * Building.h
 * - a building
 *
 */

#ifndef BUILDING_H
#define BUILDING_H

#include <iostream>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "EventResponder.h"


class Building : EventResponder
{
public:
	Building(sf::Vector2i block_size);
	~Building();

	/* Instance methods */
	void handleMouseMove(int x, int y);
 	void setPositionFromMouse(int x, int y);
	void setPos(sf::Vector2i);

	void draw(sf::RenderWindow &wind);
	
	// Override receive event method
	void receiveEvent(sf::Event *ev, EventResponder **p_e_r);

	/* Instance varables */
	bool destroyed;

protected:
	enum mode_types { PLACEMENT, PLACED } mode;
	sf::Vector2i pos;
	sf::Vector2i block_size;		// pixel size of blocks that make up the map
	sf::Vector2i size_in_blocks;	// w/h of building, in map blocks

};

#endif