/*
 * LevelView.hpp
 *
 */

#ifndef __DBT__LevelView__
#define __DBT__LevelView__

#include "W.h"

#include "Unit.hpp"
#include "Furnishing.hpp"

// LevelView-specific event types
namespace W { namespace EventType {
	extern T LV_LEFTMOUSEDOWN, LV_LEFTMOUSEUP, LV_RIGHTMOUSEDOWN, LV_RIGHTMOUSEUP, LV_MOUSEMOVE;
} }


class LevelView : public W::View {
public:
	LevelView();
	~LevelView();
	
	void processMouseEvent(W::Event *);
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
	void updatePosition(const W::size &winsize);
	
	W::DrawnRect *bgRect;
	
	int gridsize;
	int level_width, level_height;	// -1 until set with setLevelSize()
	
	void scroll(int x, int y);
	
	friend class DrawnUnit;
	friend class DrawnFurnishing;
};


class BottomUIBarView : public W::UIView {
public:
	BottomUIBarView() : W::UIView("UIViews/BottomUIBarView.lua") { }
	// Also want to add the economy, & setter for its contents
//	char econ[14];
//	sprintf(econ, "%c%d", MR_CURRENCY, *economy);
//	drawText(790, 10, W::Colour::White, econ, true);
};


class FurnishingPurchasingUIView : public W::UIView {
public:
	FurnishingPurchasingUIView() : W::UIView("UIViews/FurnishingPurchasingUIView.lua") { }
};


class HiringUIView : public W::UIView {
public:
	HiringUIView() : W::UIView("UIViews/HiringUIView.lua") { }
};


class GTTHelpView  : public W::UIView {
public:
	GTTHelpView();
	void setTimeRemaining(int);
	void setMonetaryTarget(int);
	
private:
	int timeRemaining, monetaryTarget;
	void setTimeRemainingText();
	W::DrawnText *t1, *t2, *t3, *t4, *t5, *t6, *t7, *t8, *t9;
	W::DrawnRect *r1;
};

#endif
