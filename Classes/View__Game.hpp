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
	
	void updatePosition(W::v2i);
	
	void convertEventCoords(W::Event *);
	W::EventPropagation::T scrollEvent(W::Event *);
	
	void setLevelSize(W::v2i sz) {
		level_width = sz.a;
		level_height = sz.b;
	}

  void setRemainingTime(float seconds);

	W::v2f convertGridToPixelCoords(W::v2f);
	W::v2f convertPixelToGridCoords(W::v2f);

  void customOpenGLDrawing() {
    center_if_necessary();
  }
	
protected:
	W::Rectangle *bgrect;
  W::RetroText *remaining_time_txt;
	
	int gridsize;
	int level_width, level_height;	// -1 until set with setLevelSize()
	
	void scroll(int x, int y);
  void center_if_necessary();
};

#endif
