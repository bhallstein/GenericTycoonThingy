/*
 * Generic Tycoon Thingy
 *
 * =================
 *  LevelView.hpp
 * =================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#ifndef __DBT__LevelView__
#define __DBT__LevelView__

#include "W.h"

class LevelView : public W::View {
public:
	LevelView();
	~LevelView();
	
	void updatePosition(const W::size &);
	
	void convertEventCoords(W::Event *);
	W::EventPropagation::T scrollEvent(W::Event *);
	
	void setLevelSize(const W::size &sz) {
		level_width = sz.width;
		level_height = sz.height;
	}
	W::position convertGridToPixelCoords(const W::position &);
	W::size     convertGridToPixelCoords(const W::size &);
	W::position convertPixelToGridCoords(const W::position &);
	W::size     convertPixelToGridCoords(const W::size &);
	
protected:
	W::DRect *bgRect;
	
	int gridsize;
	int level_width, level_height;	// -1 until set with setLevelSize()
	
	void scroll(int x, int y);
	
	friend class DrawnUnit;
	friend class DrawnFurnishing;
};

#endif
