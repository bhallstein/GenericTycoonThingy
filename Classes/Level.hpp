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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "types.h"
#include "NavMap.hpp"
#include "EventResponder.hpp"
#include "Placeable.hpp"
#include "Building.hpp"
#include "Unit.hpp"
#include "View.hpp"
#include "EventHandler.hpp"

using boost::property_tree::ptree;

class LevelView : public ScrollingView {
public:
	
	// Methods
	LevelView(sf::RenderWindow *, int _blocks_w, int _blocks_h, int _l_offset, int _t_offset, int _r_offset, int _b_offset);
	void draw(std::vector<Building*> *, std::vector<Placeable*> *, std::vector<Unit*> *);
	void acceptEvent(Event *);
	
};

class Level
{
public:
	Level(std::string fileName, sf::RenderWindow *_window, EventHandler *);
	~Level();

	// Methods
	void draw();

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
	ptree readLevel(std::string fileName);
	void buildLevel(ptree level_tree);
	
	// Properties
	int w, h; 					// Blocks wide/tall.
	sf::RenderWindow *window;
	EventHandler *eventHandler;
	
	NavMap *navmap;
	LevelView *levelview;
	View uiview;
	

	int framecount;	

};

#endif
