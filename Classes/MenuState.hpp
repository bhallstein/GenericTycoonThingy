/*
 * Generic Tycoon Thingy
 *
 * =================
 *  MenuState.hpp
 * =================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

/*
 * MenuState.hpp - a primitive menu state
 * 
 * Instantiates a ClickToBeginView, which is a UIView containing a single button
 * and a MenuBackgroundView, which has flashy colours.
 *
 */ 
 
#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "W.h"

class LevelState;

class MenuBackgroundView;
class ClickToBeginView;

class MenuState : public W::GameState {
public:
	MenuState();
	~MenuState();
	
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

	LevelState *levelState;
};


class ClickToBeginView : public W::UIView {
public:
	ClickToBeginView(MenuState *);
	void draw();
private:
	MenuState *menuState;
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
