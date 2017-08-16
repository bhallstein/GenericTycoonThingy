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
#include "GameState.hpp"


class Game
{
public:
	Game(sf::RenderWindow *, SettingsManager *);
	~Game();

	// Methods
	void run();
	
	void pushState(GameState *);
	void popState();
	void stateFinished(GameState *, Returny);
	
	// Events/Update/Drawing - passed to active State
	void sendEvents();
	void update();
	void draw();
	
	SettingsManager* settings;

protected:
	// Properties
	std::vector<GameState *> states;
	bool resume;
	Returny returny;
	
	sf::RenderWindow *window;
	
};

#endif
