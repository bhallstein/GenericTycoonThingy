/*
 * Building.h
 *
 */

#ifndef BUILDING_H
#define BUILDING_H

#include <iostream>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>


/* EventResponder
   - an abstract class, for subclassing, to allow various objects to handle events.
   - implement the subclass’s specific event behaviour by overriding receiveEvent(). */

class EventResponder
{
public:
	EventResponder();
	~EventResponder();
	
	virtual void receiveEvent(sf::Event *ev, EventResponder **p_e_r);

protected:
	
};

/* Building
   - a building */

class Building : EventResponder
{
public:
	Building(sf::Vector2i block_size);
	~Building();

	void handleMouseMove(int x, int y);
 	void setPositionFromMouse(int x, int y);
	void setPos(sf::Vector2i);

	void draw(sf::RenderWindow &wind);
	
	/* Override receive event method */
	void receiveEvent(sf::Event *ev, EventResponder **p_e_r);

protected:
	enum mode_types { PLACEMENT, PLACED } mode;
	sf::Vector2i pos;
	sf::Vector2i block_size;		// pixel size of blocks that make up the map
	sf::Vector2i size_in_blocks;	// w/h of building, in map blocks
};

#endif