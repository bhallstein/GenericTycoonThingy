/*
 * Game.h - Game class, the main class for running the game, really.
 *
 */

#ifndef GAME_H
#define GAME_H

#include <vector>

#include "types.h"
#include "Returny.hpp"

class W;
class GameState;
class Event;
class SettingsManager;

class Game
{
public:
	Game(W *);
	~Game();

	// Methods
	
	// Events/Update/Drawing
	void sendEvents(std::vector<Event*> *);
	bool update();
	void draw();
	
	// State methods
	void pushState(GameState *);
	void popState();
	void stateFinished(GameState *, Returny);
	
	SettingsManager *settings;

protected:
	// Properties
	std::vector<GameState *> states;
	bool resume;
	Returny returny;
	
	W *theW;
	int prev_w, prev_h;
	
	bool finishedIntro;
	
};

#endif
