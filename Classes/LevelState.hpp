/*
 * LevelState.hpp
 *
 */

#ifndef __DBT__LevelState__
#define __DBT__LevelState__

#include "W.h"

class LevelMap;
class Building;

/* View-related forward class declarations */
class LevelView;
class BottomUIBarView;
class FurnishingPurchasingUIView;
class HiringUIView;
class GTTHelpView;


class LevelState : public W::GameState {
public:
	LevelState();
	~LevelState();
	
	W::EventPropagation::T keyEvent(W::Event *);
	
	void update();
	void resume(W::Returny *);

	void pause();
	void unpause();
	
	bool addPlayerMoneys(int);
	
	bool loadLevel(const std::string &levelName);
	bool saveLevel(const std::string &saveName);
	
private:
	// Views
	LevelView *levelView;
	
	// Map
	LevelMap *levelMap;
	
	// Level-specific data
	int monetaryTarget;
	int timeLimit;
	
	// Player state
	int playerMoneys;
	
	// Building editing
	Building *currentlyEditedBuilding;
	
	// Time
	bool paused;
	int framecount;
	W::Timer *realtimetimer;
	float realtimetime;	// Number of real-time seconds elapsed while level running
	int timeRemaining;	// integer time remaining calculated from realtimetime and the level's time limit - passed to levelview for display, don't use for calculations
};

#endif