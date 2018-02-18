/*
 * Generic Tycoon Thingy
 *
 * =====================
 *  State__TopMenu.hpp
 * =====================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

/*
 * State__TopMenu.hpp - a primitive menu state
 * 
 * Instantiates a ClickToBeginView, which is a UIView containing a single button
 * and a MenuBackgroundView, which has flashy colours.
 *
 */ 
 
#ifndef State__TopMenu_H
#define State__TopMenu_H

#include "W.h"

class State__Game;

class View__MenuBG;
class View__MenuUI;


class State__TopMenu : public W::GameState {
public:
	State__TopMenu();
	~State__TopMenu();
	
	// Methods
	void resume(W::Returny *);
	void update();
	
	W::EventPropagation::T keyEvent(W::Event *);
	W::EventPropagation::T uiEvent(W::Event *);
	
	void startLevel(const std::string &levelName);
	void startLevelOne();
	
protected:
	View__MenuBG *view__menuBG;
  View__MenuUI *view__menuUI;

	State__Game *state__game;
};


class View__MenuUI : public W::UIView {
public:
	View__MenuUI();
private:
  W::DText *t1;
  W::DText *t2;
};


#endif
