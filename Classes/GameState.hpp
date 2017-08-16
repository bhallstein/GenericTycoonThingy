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
  virtual void Init(sf::RenderWindow* window,EventHandler* eventHandler) = 0;
  virtual void Cleanup() = 0;

  virtual void Pause() = 0;
  virtual void Resume() = 0;

  //Events/Update/Draw - pass a pointer to Game through in case we need it
  virtual void HandleEvents(Game* g,Event* event) = 0;
  virtual void Update(Game* g) = 0;
  virtual void Draw(Game* g) = 0;

  //Changing state actually gets Game to do it ;)
  void ChangeState(Game* g, GameState* state)
  {
    g->ChangeState(state);
  }

protected:
	GameState() { }  //Protected constructor allows for singletons
};

#endif