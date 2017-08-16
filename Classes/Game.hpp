/*
 * Game.h - Game class, the main class for running the game, really.
 *
 */

#ifndef GAME_H
#define GAME_H

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "types.h"

#include "SettingsManager.hpp"
#include "GameMap.hpp"
#include "Level.hpp"
#include "Event.hpp"


class Game
{
public:
	Game();
	~Game();

	// Methods
	void Run();

	// Properties
	SettingsManager Settings;

protected:
	
	// Properties
	sf::RenderWindow DBTWindow;
	int w, h; 				// Blocks wide/tall
	int block_w, block_h;	// Pixels per block
	GameMap gamemap;
};

#endif
