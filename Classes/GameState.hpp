/*
 * GameState.hpp - base class for creating State objects
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

#include <list>
#include <map>
#include <iostream>

#include "Event.hpp"
#include "ResponderMap.hpp"

class Game;
class W;
class Returny;
class EventResponder;

class GameState {
public:
	GameState(Game *, W *);
	~GameState();
	
	// State-related virtual methods - subclasses must override
	virtual void pause() = 0;
	virtual void resume(Returny *) = 0;
	virtual void update() = 0;
	virtual void draw() = 0;
	
	void handleEvent(Event *ev);		// Send the event on, via the gamestate's responderMap
	virtual void handleCloseEvent();	// By default, initiates the killer returny tango: override to customise
	
	virtual void setResolution(int _w, int _h);		// By default, calls responderMap.setSize()
	
	// Properties
	bool translucent;	// If true, the previous state will be drawn too. For in-game menus etc. Default: false
	
protected:
	// Properties
	Game *game;
	W *theW;
	
	ResponderMap responderMap;
};

#endif
