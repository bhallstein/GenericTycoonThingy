/*
 * GameState.hpp - abstract base class for creating State objects
 *
 * Note on pseudo-singletons:
 *  - For classes you want to ensure have only one instance, can prevent this from happening by providing a static `instances` property.
 *  - Then, in the constructor of your class, if (instances > 0), you can throw an exception.
 *		- Remember to initialise the static immediately after your class interface: `MyState::instances = 0;`
 *      - The static must be in the derived class, because otherwise all derived classes will share the base class’s static.
 *      - Also, it is good practice to always refer to your static as Class::property, since it makes the staticity clear to the reader.
 *      - See ben.am/eK
 */

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "Game.hpp"
#include "EventHandler.hpp"


class GameState
{
public:
	GameState(Game *_game, sf::RenderWindow *_win) : game(_game), window(_win), translucent(false) { }
	~GameState() { }
	
	// State-related virtual methods – subclasses must override
	virtual void reset() = 0;
	virtual void pause() = 0;
	virtual void resume() = 0;
	virtual void handleEvent(Event *) = 0;
	virtual void update() = 0;
	virtual void draw() = 0;
	
	void _handleEvent(Event *ev) {			// Wrapper for handleEvent:
		if (ev->type == KEYPRESS)			//   This is what gets called by Game, and passes mouse events to the eventhandler.
			handleEvent(ev);				//   Thus, a GameState subclass’s handleEvent() override only need respond to key presses.
		else
			eventHandler.dispatchEvent(ev);
	}
	
	// Properties
	bool translucent;	// Whether to draw the previous state too. Set to true for in-game menus, popovers & so on.

protected:
	// Properties
	Game *game;
	sf::RenderWindow *window;
	EventHandler eventHandler;
	
};

#endif
