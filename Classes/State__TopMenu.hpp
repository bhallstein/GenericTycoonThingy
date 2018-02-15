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

class MenuBackgroundView;
class ClickToBeginView;

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
	MenuBackgroundView *menubackgroundview;
	ClickToBeginView *clicktobeginview;

	State__Game *state__game;
};


class ClickToBeginView : public W::UIView {
public:
	ClickToBeginView();
	void draw();
private:
	State__TopMenu *state__topMenu;
	W::DText *t1, *t2;
};


//struct colour_square;
//
//class MenuBackgroundView : public W::View {
//public:
//	MenuBackgroundView(W::Window *);
//	~MenuBackgroundView();
//	void draw();
//private:
//	int *colour_squares;
//	W::Colour *colour_cycle_array;
//	int colour_cycle_length;
//	
//	int framecount;
//	
//	enum _modes {
//		MNONE,
//		MRANDOM, MSTRIPESH, MSTRIPESREV, MFADE, /*MSTRIPESD,*/ MPULSE, /*MTURNY,*/
//		MTRANSWIPE, MTRANSCIRC, MTRANSBARS,
//		MLAST
//	} mode;
//	void switchMode();
//	
//	void incrementColour(int *sq, int n = 1) {
//		*sq += n;
//		if (*sq >= colour_cycle_length) *sq -= colour_cycle_length;
//	}
//	void decrementColour(int *sq, int n = 1) {
//		*sq -= n;
//		if (*sq < 0) *sq += colour_cycle_length;
//	}
//	
//	float alpha;
//	bool mrandom_prerandomised;
//	Menu *menu;
//	int *mtransbars_heights;
//};

#endif
