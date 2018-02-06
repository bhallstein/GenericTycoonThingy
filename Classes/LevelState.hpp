/*
 * Generic Tycoon Thingy
 *
 * ==================
 *  LevelState.hpp
 * ==================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#ifndef __DBT__LevelState__
#define __DBT__LevelState__

#include "W.h"

class LevelMap;
class Building;

/* View-related forward class declarations */
class LevelView;
class View__BottomBar;
class View__Help;
class View__Hiring;
class View__FurnishingPurchasing;


class LevelState : public W::GameState {
public:
	LevelState();
	~LevelState();
	
	W::EventPropagation::T keyEvent(W::Event *);
	W::EventPropagation::T buttonEvent(W::Event *);
	
	void update();
	void resume(W::Returny *);

	void pause();
	void unpause();
	
	bool loadLevel(const std::string &levelName);
	bool saveLevel(const std::string &saveName);

	void openView_help();
	void closeView_help();

	void openView_hiring();
	void closeView_hiring();

  void openView_furnishingPurchasing();
  void closeView_furnishingPurchasing();
	
private:
	// Views
	LevelView *levelView;
	View__BottomBar *view__btmUIBar;
	View__Help      *view__help;
	View__Hiring    *view__hiring;
  View__FurnishingPurchasing *view__furnishingPurchasing;
	
	// Map
	LevelMap *levelMap;
	
	// Time
	bool paused;
	W::Timer *realtimetimer;
	float realtimetime;	// Number of real-time seconds elapsed while level running
	int timeRemaining;	// integer time remaining calculated from realtimetime and the level's time limit - passed to levelview for display, don't use for calculations
};

#endif
