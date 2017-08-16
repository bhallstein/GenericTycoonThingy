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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "types.h"
#include "GameState.hpp"
#include "EventResponder.hpp"

class Game;
class W;
class ResponderMap;
class Button;
class NavMap;
class Building;
class Placeable;
class Unit;

class LevelView;
class UIBarView;

using boost::property_tree::ptree;

class Level : public GameState, public EventResponder {
public:
	Level(Game *, W *, std::string path);
	~Level();
	
	// Methods
	
	// GameState function implementations
	void reset();
	void pause();
	void resume(Returny *);
	void update();
	void draw();
	void setResolution();

	void handleCloseEvent();		// Will need to prompt for save, probably
	void receiveEvent(Event *);		// Level is also an eventresponder, so can itself subscribe directly to events
	void buttonClick(Button *);
	
	void setResolution(int _w, int _h);
	
	// Top-level-object stuff
	Unit* createUnit(int atX, int atY);
	Building* createBuilding(int atX, int atY);
	void createPlaceable();
	
	void destroyThings();
	void destroyAllThings();
	
	// Properties
	int columns, rows;
	std::vector<Building*> buildings;
	std::vector<Placeable*> placeables;
	std::vector<Unit*> units;
	
protected:
	// Methods
	ptree readLevel(std::string fileName);
	void buildLevel(ptree level_tree);
	
	// Properties
	int w, h; 					// Blocks wide/tall.
	
	NavMap *navmap;
	ResponderMap *levelResponderMap;
	LevelView *levelview;
	UIBarView *uibarview;
	
	int framecount;
	
};


#include "View.hpp"
#include "JenniferAniston.hpp"

class LevelView : public View {
public:
	// Methods
	LevelView(
		W *, JenniferAniston &,
		ResponderMap *_levelRM, std::vector<Building*> *, std::vector<Placeable*> *, std::vector<Unit*> *,
		int _level_width, int _level_height
	);
	void draw();
	void drawMappedObj(MappedObj *obj);	// Utility fn for drawing objects
	void processMouseEvent(Event *);
	void scroll(direction);
	
	// Properties
	int gridsize;
	int level_width, level_height;		// how many blocks wide/tall the level is
	int scroll_x, scroll_y;
	ResponderMap *levelResponderMap;
	std::vector<Building*> *buildings;
	std::vector<Placeable*> *placeables;
	std::vector<Unit*> *units;
	
};


#include "ButtonReceiver.hpp"

class UIBarView : public View, public ButtonReceiver {
public:
	UIBarView(W *, JenniferAniston &, Level *);
	~UIBarView();
	
	// Methods
	void buttonClick(Button *);
	void draw();
	void updatePosition();
	void processMouseEvent(Event *);
	
	// Properties
	Level *level;
	Button *createplaceable_btn;
	std::vector<Button *> buttons;
	ResponderMap buttonMap;
};

#endif
