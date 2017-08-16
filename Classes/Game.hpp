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

#include "SettingsManager.hpp"
#include "Event.hpp"
#include "EventHandler.hpp"

class GameState;

class Game
{
public:
	Game(sf::RenderWindow *);
	~Game();

	// Methods
	void Cleanup();

	void Run();

	//State Methods
	void ChangeState(GameState *);
	void PushState(GameState *);
	void PopState();

	//Events/Update/Draw - usually passed to active State
	void HandleEvents(Event event,sf::Event sf_event); 
	void Update();
	void Draw();

	bool Running() { return is_running; }
	void Quit() { is_running = false; }

	SettingsManager settings;

protected:
	bool is_running;

	std::vector<GameState*> states; //the stack of states

	sf::RenderWindow *window;
	EventHandler eventHandler;
};

#endif
