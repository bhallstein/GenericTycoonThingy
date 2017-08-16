/*
 * GameState.hpp - Header for a single "State" object
 *
 */

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Game.hpp"

class GameState
{
public:
  virtual void init(sf::RenderWindow* window,EventHandler* eventHandler) = 0;
  virtual void reset() = 0;
               
  virtual void pause() = 0;
  virtual void resume() = 0;
               
  // Events/Update/Drawing - pass a pointer to Game through in case we need it
  virtual void handleEvents(Game* g,Event* event) = 0;
  virtual void update(Game* g) = 0;
  virtual void draw(Game* g) = 0;

  // Pass state changes through to Game
  void changeState(Game* g, GameState* state) { g->changeState(state); }

protected:
	GameState() { }  //Protected constructor allows for singletons
};

#endif
