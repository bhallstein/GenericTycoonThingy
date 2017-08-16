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
#include "MappedObj.hpp"
#include "Level.hpp"



class MenuView;
class Button;
class Level;

class Menu : public GameState
{
public:
	Menu(Game *, sf::RenderWindow *);
	~Menu();
	
	// Methods
	void reset();
	void pause();
	void resume(Returny *);
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


class Button : public MappedObj
{
public:
	Button(Menu *, int _x, int _y);
	~Button();
	
	// Methods
	void receiveEvent(Event *);
	sf::Color col();
	
protected:
	bool hover;
	Menu *menu;
	
};


class MenuView : public View
{
public:
	MenuView(sf::RenderWindow *, int _blocks_w, int _blocks_h, int _l_offset, int _t_offset, int _r_offset, int _b_offset);
	void draw(std::vector<Button*> *);
	
};

#endif
