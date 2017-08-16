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
#include "EventHandler.hpp"
#include "SettingsManager.hpp"

//Forward Declarations
class GameState;

class Game
{
public:
	Game(sf::RenderWindow *,SettingsManager *);
	~Game();

	// Methods
	void run();
	
	void cleanStates();
	void changeState(GameState *);
	void pushState(GameState *);
	void popState();
	
	// Events/Update/Drawing - passed to active State
	void handleEvents();
	void update();
	void draw();
	
	bool running() { return is_running; }
	void quit();
	
	SettingsManager* settings;

protected:
	bool is_running;

	std::vector<GameState*> states; //the stack of states

	sf::RenderWindow *window;
	EventHandler eventHandler;
};

#endif
