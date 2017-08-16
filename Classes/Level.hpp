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
#include "SpawnPoint.hpp"
//#include "LevelScore.hpp" // TODO: re-add levelscore

class Building;
class Furnishing;
class Unit;
class Behaviour;

class LevelView;
class UIBarView;
class FurnishingPurchasingUIView;
class HiringUIView;
class GTTHelpView;

class LevelScore;

namespace Ogre { class Timer; }

// Register level event types
namespace W { namespace EventType {
	extern T
	LEVEL_LEFTMOUSEDOWN,
	LEVEL_LEFTMOUSEUP,
	LEVEL_RIGHTMOUSEDOWN,
	LEVEL_RIGHTMOUSEUP,
	LEVEL_MOUSEMOVE;
} }

class Level : public W::GameState, public W::EventResponder {
public:
	Level(W::Window *, std::string path);
	~Level();
	
	// GameState function implementations
	void resume(W::Returny *);
	void update();
	void setResolution();
	
	void pause();
	void unpause();

	void handleCloseEvent();		// Will need to prompt for save, probably
	void scrollEvent(W::Event *);
	void keyEvent(W::Event *);
	void buttonEvent(W::Event *);
	
	void setResolution(int _w, int _h);
	
	// TLO stuff
	Unit*       createUnit(int atX, int atY, const char *type);
	Building*   createBuilding(W::position &, const char *type, std::vector<W::rect> *plan);
	Furnishing* createFurnishing(const char *type, bool placeableMode = true, int atX = 0, int atY = 0);
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
	std::vector<Furnishing*> doors;
	std::vector<Unit*>       units;
	std::vector<Unit*>       staff;
	std::vector<Behaviour*>  behaviours;
	std::vector<SpawnPoint*> spawnPoints;
	
protected:
	void buildLevel(std::string filename);
	
	W::Window *win;
	
	int w, h; 					// Blocks wide/tall.
	
	W::NavMap *navmap;
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
	bool paused;
	Ogre::Timer *realtimetimer;
	float realtimetime;	// Number of real-time seconds elapsed while level running
	int timeRemaining;	// integer time remaining calculated from realtimetime and the level's time limit - passed to levelview for display, don't use for calculations
	
	std::list<std::string> allowedBuildings;
};


class LevelView : public W::View {
public:
	// Methods
	LevelView(
		W::Window *, W::EventHandler *,
		std::vector<Building*> *, std::vector<Furnishing*> *, std::vector<Unit*> *_units, std::vector<Unit*> *_staff,
		int _level_width, int _level_height,
		int *_time_remaining,
		W::NavMap *
	);
	
//	void setMousePos(W::position p) {
//		mousepos = p;
//	}
//	W::position mousepos;
	
	void draw();
	inline void drawTLOAt(TLO *, W::position &);
	void drawTLO(TLO *);				// Utility fn for drawing our top-level objects
	void drawPM(PlaceableManager *);	// ...& for drawing those that may manage a placeable
	void processMouseEvent(W::Event *);
	void scroll(Direction::T);
	
	// Properties
	int gridsize;
	int level_width, level_height;		// how many blocks wide/tall the level is
	int scroll_x, scroll_y;
	int *time_remaining;

	std::vector<Building*>   *buildings;
	std::vector<Furnishing*> *furnishings;
	std::vector<Unit*>       *units;
	std::vector<Unit*>       *staff;
	
	W::EventHandler *eh;
	W::NavMap *nm;
};


class UIBarView : public W::UIView {
public:
	UIBarView(W::Window *, W::EventHandler *, int *_econ);
	void draw();
private:
	int *economy;
};

class FurnishingPurchasingUIView : public W::UIView {
public:
	FurnishingPurchasingUIView(W::Window *, W::EventHandler *, std::vector<std::string> *_furnishingTypes);
	void draw();
protected:
	std::vector<std::string> *furnishingTypes;
};

class HiringUIView : public W::UIView {
public:
	HiringUIView(W::Window *, W::EventHandler *);
	void draw();
};

class GTTHelpView : public W::UIView {
public:
	GTTHelpView(W::Window *, W::EventHandler *);
	void draw();
};

#endif
