/*
 * Menu.hpp - a menu class
 * 
 * This will be a base class for specific menus.
 * Am just implementing a single menu now, to work out what it will look like.
 *
 */

#ifndef MENU_H
#define MENU_H

#include <vector>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "GameState.hpp"
#include "View.hpp"
#include "EventResponder.hpp"
#include "Level.hpp"

class Menu;

class Button : public EventResponder
{
public:
	Button(Menu *, int _x, int _y);
	~Button();
	
	// Methods
	void receiveEvent(Event *);
	char col();
	
protected:
	bool hovered;
	Menu *menu;
	
};


class MenuView : public View
{
public:
	MenuView(sf::RenderWindow *, int _blocks_w, int _blocks_h, int _l_offset, int _t_offset, int _r_offset, int _b_offset);
	void draw(std::vector<Button*> *);
	
};


class Menu : public GameState
{
public:
	Menu(Game *, sf::RenderWindow *);
	~Menu();
	
	// Methods
	void reset();
	void pause();
	void resume();
	void handleEvent(Event *);
	void update();
	void draw();
	
	void startLevel(std::string path);
	
protected:
	// Properties
	MenuView *menuview;
	std::vector<Button*> buttons;
	
	Level *level;		// This or a future LevelLoadinator will clearly one day be specific to a subclass
	
};

#endif