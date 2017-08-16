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
#include "Level.hpp"
#include "Event.hpp"
#include "EventHandler.hpp"


class Game
{
public:
	Game(sf::RenderWindow *);
	~Game();

	// Methods
	void Run();

	// Properties
	SettingsManager settings;

protected:
	
	// Properties
	sf::RenderWindow *window;
	EventHandler eventHandler;
};

#endif
