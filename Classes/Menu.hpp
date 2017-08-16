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
#include "W.h"

class MenuView;
class Level;

class Menu : public W::GameState, public W::EventResponder {
public:
	Menu(W::Window *);
	~Menu();
	
	// Methods
	void resume(W::Returny *);
	void update();
	
	void receiveEvent(W::Event *);
	
	void startLevel(std::string path);
	void startLevelOne();
	
protected:
	// Properties
	MenuView *menuview;
	
	Level *level;		// This (or a future LevelLoadinator) will clearly one day be specific to a subclass
	W::Window *win;
};

class MenuView : public W::UIView {
public:
	MenuView(W::Positioner *, W::Window *, W::EventHandler *);
	void draw();
};

#endif
