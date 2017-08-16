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

class Building
{
public:
	Building();
	~Building();

	void handleMouseMove(int x, int y);
	bool placeAt(int x, int y);

	void draw(sf::RenderWindow &wind);

protected:
	enum mode_types { PLACEMENT, PLACED } mode;
	int posX, posY;
	int width, height;
};

#endif