/*
 * Game.h - Game class, the main class for running the game, really.
 *
 */

#ifndef GAME_H
#define GAME_H

#include "Level.h"
#include "SettingsManager.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>


class Game
{
public:
	Game();
	~Game();

	// Instance methods
	void Run();

	// Instance Properties
	SettingsManager Settings;

protected:
	sf::RenderWindow DBTWindow;
	int pixel_width, pixel_height;
	
};

#endif