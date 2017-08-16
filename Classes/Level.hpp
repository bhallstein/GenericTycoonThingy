/*
 * Level.hpp - Controls a level, including all the objects therein
 *
 */

#ifndef LEVEL_H
#define LEVEL_H

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <ctime>

#include "types.hpp"
#include "GameState.hpp"
#include "EventResponder.hpp"
#include "SpawnPoint.hpp"
#include "LevelScore.hpp"

class Game;
class W;
class ResponderMap;
class Button;
class NavMap;
class Building;
class Furnishing;
class Unit;
class Behaviour;

class LevelView;
class UIBarView;
class FurnishingPurchasingUIView;
class HiringUIView;
class GTTHelpView;
namespace Ogre { class Timer; }

class Level : public GameState, public EventResponder {
public:
	Level(Game *, W *, std::string path);
	~Level();
	
	// GameState function implementations
	void reset();
	void pause();
	void resume(Returny *);
	void update();
	void draw();
	void setResolution();

	void handleCloseEvent();		// Will need to prompt for save, probably
	void receiveEvent(Event *);		// Level is also an eventresponder, so can itself subscribe directly to events
	
	void setResolution(int _w, int _h);
	
	// TLO stuff
	Unit*       createUnit(int atX, int atY, const char *type);
	Building*   createBuilding(int atX, int atY, const char *type, std::vector<intcoord> *groundplan, std::vector<door> *doors);
	Furnishing* createFurnishing(const char *type);
	Behaviour*  createBehaviour(const char *type);
	void purchaseFurnishing(const char *type);
	void hireStaff(const char *type);
	
	void destroyThings();
	void destroyAllThings();
	
	Building* randomBuildingWithType(const char *);
	Building* buildingAtLocation(int x, int y);
	
	// View stuff
	void openFurnishingPurchasingView(Building *);
	void closeFurnishingPurchasingView();
	Building *currentlyEditedBuilding;
	void openHiringView();
	void closeHiringView();
	void openHelpView();
	void closeHelpView();

	// Money
	void payPlayer(int);
	bool chargePlayer(int);		//returns false if player can't be charged (i.e. not enough positive balance to complete transaction)
	void decreaseMoney(int);	//as chargePlayer() but without the checks, since some actions may decrease even if Player has negative balance (e.g. monthly expenditure)
	
	// Properties
	int columns, rows;
	std::vector<Building*>   buildings;
	std::vector<Furnishing*> furnishings;
	std::vector<Unit*>       units;
	std::vector<Unit*>       staff;
	std::vector<Behaviour*>  behaviours;
	std::vector<SpawnPoint*> spawnPoints;
	
protected:
	// Methods
	void buildLevel(std::string filename);
	
	// Properties
	int w, h; 					// Blocks wide/tall.
	
	NavMap *navmap;
	ResponderMap *levelResponderMap;
	LevelView *levelview;
	UIBarView *uibarview;
	FurnishingPurchasingUIView *furnishingPurchasingView;
	HiringUIView *hiringUIView;
	GTTHelpView *helpView;
	LevelScore *levelScore;
	
	int money;
	int moneyLimit; //potential victory condition
	int timeLimit;	//potential time limit

	int framecount;
	Ogre::Timer *realtimetimer;
	float realtimetime;	// Number of real-time seconds elapsed while level running
	int timeRemaining;	// integer time remaining calculated from realtimetime and the level's time limit - passed to levelview for display, don't use for calculations
	
	std::list<std::string> allowedBuildings;
};


#include "View.hpp"
#include "JenniferAniston.hpp"

class LevelView : public View {
public:
	// Methods
	LevelView(
		W *, JenniferAniston &,
		ResponderMap *_levelRM,
		std::vector<Building*> *, std::vector<Furnishing*> *, std::vector<Unit*> *_units, std::vector<Unit*> *_staff,
		int _level_width, int _level_height,
		int *_time_remaining
	);
	void draw();
	void drawMappedObj(MappedObj *obj);	// Utility fn for drawing objects
	void processMouseEvent(Event *);
	void scroll(Direction::Enum);
	
	// Properties
	int gridsize;
	int level_width, level_height;		// how many blocks wide/tall the level is
	int scroll_x, scroll_y;
	ResponderMap *levelResponderMap;
	int *time_remaining;

	std::vector<Building*>   *buildings;
	std::vector<Furnishing*> *furnishings;
	std::vector<Unit*>       *units;
	std::vector<Unit*>       *staff;
};


#include "UIView.hpp"

class UIBarView : public UIView {
public:
	UIBarView(W *, JenniferAniston &, ResponderMap *, int *_econ);
	void draw();
private:
	int *economy;
};

class FurnishingPurchasingUIView : public UIView {
public:
	FurnishingPurchasingUIView(W *, JenniferAniston &, ResponderMap *, std::vector<std::string> *_furnishingTypes);
	void draw();
protected:
	std::vector<std::string> *furnishingTypes;
};

class HiringUIView : public UIView {
public:
	HiringUIView(W *, JenniferAniston &, ResponderMap *);
	void draw();
};

class GTTHelpView : public UIView {
public:
	GTTHelpView(W *, JenniferAniston &, ResponderMap *);
	void draw();
};

#endif
