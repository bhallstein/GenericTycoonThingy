/*
 * Generic Tycoon Thingy
 *
 * =================
 *  View__Game.hpp
 * =================
 *
 * Copyright (C) 2012 - Ben Hallstein
 * All rights reserved
 *
 */

#ifndef __DBT__View__Game
#define __DBT__View__Game

#include "W.h"

class View__Game : public W::View {
public:
	View__Game();
	~View__Game();
	
	void updatePosition(const W::size &);
	
	void convertEventCoords(W::Event *);
	W::EventPropagation::T scrollEvent(W::Event *);
	
	void setLevelSize(const W::size &sz) {
		level_width = sz.width;
		level_height = sz.height;
	}

  void setRemainingTime(float seconds);

	W::position convertGridToPixelCoords(const W::position &);
	W::size     convertGridToPixelCoords(const W::size &);
	W::position convertPixelToGridCoords(const W::position &);
	W::size     convertPixelToGridCoords(const W::size &);
	
protected:
	W::DRect *bgRect;
  W::DText *remaining_time_txt;
	
	int gridsize;
	int level_width, level_height;	// -1 until set with setLevelSize()
	
	void scroll(int x, int y);
};

#endif
