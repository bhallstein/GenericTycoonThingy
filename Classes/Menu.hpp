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
#include "ButtonReceiver.hpp"

class Game;
class W;
class Level;
class Button;

class MenuView;

class Menu : public GameState, public EventResponder, public ButtonReceiver {
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
	void buttonClick(Button *);
	
	void startLevel(std::string path);
	
protected:
	// Properties
	MenuView *menuview;
	std::vector<Button*> buttons;
	Button *startlevel_btn;
	
	Level *level;				// This (or a future LevelLoadinator) will clearly one day be specific to a subclass
	ResponderMap buttonMap;		// Is there a need for a View subclass which manages a RM?
	
};


#include "View.hpp"

class MenuView : public View {
public:
	MenuView(W *, JenniferAniston &, ResponderMap *_buttonMap, std::vector<Button*> *);
	void draw();
	void processMouseEvent(Event *);
	
	ResponderMap *buttonMap;
	std::vector<Button *> *buttons;
};

#endif
