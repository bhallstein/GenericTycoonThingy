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

#define NO_PREV_POSITION -27831


class Building : EventResponder
{
public:
	Building(sf::Vector2i block_size);
	~Building();

	/* Instance methods */
	void handleMouseMove(int x, int y);
 	void setPositionFromMouse(int x, int y);
	void getBounds(int *x1, int *y1, int *x2, int *y2);

	void draw(sf::RenderWindow &wind);
	
	// Override receive event method
	void receiveEvent(sf::Event *ev, EventResponder **p_e_r);

	/* Instance varables */
	sf::Vector2i pos;
	sf::Vector2i size_in_blocks;

	bool destroyed;
	bool position_updated;
	sf::Vector2i prev_position;

protected:
	enum mode_types { PLACEMENT, PLACED } mode;
	sf::Vector2i block_size;		// pixel size of blocks that make up the map
	
};

#endif