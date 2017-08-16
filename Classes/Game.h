/*
 * Game.h - Game class, the main class for running the game, really.
 *
 */

#ifndef GAME_H
#define GAME_H

#include "SettingsManager.h"
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "Level.h"
#include "Event.h"

#include "types.h"


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
	int w, h; 				// Blocks wide/tall
	int block_w, block_h;	// Pixels per block
};

#endif
