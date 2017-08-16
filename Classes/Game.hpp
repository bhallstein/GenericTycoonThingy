/*
 * Game.h - Game class, the main class for running the game, really.
 *
 */

#ifndef GAME_H
#define GAME_H

#include <vector>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "types.h"
#include "Event.hpp"
#include "SettingsManager.hpp"
#include "Menu.hpp"

// Forward Declarations
class GameState;

class Game
{
public:
	Game(sf::RenderWindow *, SettingsManager *);
	~Game();

	// Methods
	void run();
	
	void pushState(GameState *);
	void popState();
	void popAllStates();
	
	// Events/Update/Drawing - passed to active State
	void sendEvents();
	void update();
	void draw();
	
	void quit();
	
	SettingsManager* settings;

protected:
	// Properties
	std::vector<GameState *> states;
	bool should_quit;
	
	sf::RenderWindow *window;
	
};

#endif
