/*
 * Menu.hpp - a menu class
 * 
 * This will presumably be a base class used for creating specific menus.
 * Just implementing a single menu now, to find out what it might entail.
 *
 */

#ifndef MENU_H
#define MENU_H

#include <vector>

#include "GameState.hpp"
#include "EventResponder.hpp"

class Game;
class W;
class Level;
class Button;

class MenuView;

class Menu : public GameState, public EventResponder {
public:
	Menu(Game *, W *);
	~Menu();
	
	// Methods
	void reset();
	void pause();
	void resume(Returny *);
	void update();
	void draw();
	void setResolution(int _w, int _h);
	
	void receiveEvent(Event *);
	
	void startLevel(std::string path);
	void startLevelOne();
	
protected:
	// Properties
	MenuView *menuview;
	Level *level;		// This (or a future LevelLoadinator) will clearly one day be specific to a subclass
	
};


#include "UIView.hpp"

class MenuView : public UIView {
public:
	MenuView(W *, JenniferAniston &, ResponderMap *);
	void draw();
};

#endif
