/*
 * Level.h - Controls a level, including all the objects therein
 *
 */
#ifndef LEVEL_H
#define LEVEL_H

#include <iostream>
#include <vector>
#include <list>
#include <string>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "LuaHelper.hpp"
#include "types.h"
#include "NavMap.hpp"
#include "EventResponder.hpp"
#include "Placeable.hpp"
#include "Building.hpp"
#include "Unit.hpp"
#include "View.hpp"
#include "EventHandler.hpp"
#include "GameState.hpp"
#include "Game.hpp"

class LevelView;

class Level : public GameState
{
public:
	Level(Game *, sf::RenderWindow *, std::string path);
	~Level();
	
	// Methods
	
	// GameState overrides
	void reset();
	void pause();
	void resume(Returny *);
	void handleEvent(Event *);
	void update();
	void draw();
	
	// Top-level-object stuff
	Unit* createUnit(int atX, int atY);
	Building* createBuilding(int atX, int atY);
	void createPlaceable();
	
	void updateObjects();
	void destroyThings();
	void destroyAllThings();
	
	// Properties
	int columns, rows;
	std::vector<Building*> buildings;
	std::vector<Placeable*> placeables;
	std::vector<Unit*> units;
	
protected:
	// Methods
	void buildLevel(std::string filename);
	
	// Properties
	int w, h; 					// Blocks wide/tall.
	
	NavMap *navmap;
	LevelView *levelview;
	View *uiview;
	
	int framecount;

};


class LevelView : public ScrollingView
{
public:
	// Methods
	LevelView(sf::RenderWindow *, int _blocks_w, int _blocks_h, int _l_offset, int _t_offset, int _r_offset, int _b_offset);
	void draw(std::vector<Building*> *, std::vector<Placeable*> *, std::vector<Unit*> *);
	
};

#endif
