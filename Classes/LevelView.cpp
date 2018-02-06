/*
 * Generic Tycoon Thingy
 *
 * =================
 *  LevelView.cpp
 * =================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#include "LevelView.hpp"

/********************************/
/*** LevelView implementation ***/
/********************************/

LevelView::LevelView() :
	View(new W::Positioner(W::Corner::TopLeft,
                         W::PosType::Fixed,
                         W::PosType::Fixed,
                         W::PosType::Proportional,
                         W::PosType::Proportional,
                         0, 0, 1, 1)),
	gridsize(20),
	level_width(-1), level_height(-1)
{
	bgRect = new W::DRect(this, rct.pos, rct.sz, W::Colour::White);
	
	// Screenedge scrolling
	W::Callback scrCb(&LevelView::scrollEvent, this);
	W::Messenger::subscribe(W::EventType::ScreenEdgeLeft,   scrCb);
	W::Messenger::subscribe(W::EventType::ScreenEdgeRight,  scrCb);
	W::Messenger::subscribe(W::EventType::ScreenEdgeTop,    scrCb);
	W::Messenger::subscribe(W::EventType::ScreenEdgeBottom, scrCb);
}
LevelView::~LevelView()
{
	W::Messenger::unsubscribe(W::EventType::ScreenEdgeLeft,   this);
	W::Messenger::unsubscribe(W::EventType::ScreenEdgeRight,  this);
	W::Messenger::unsubscribe(W::EventType::ScreenEdgeTop,    this);
	W::Messenger::unsubscribe(W::EventType::ScreenEdgeBottom, this);
}

void LevelView::updatePosition(const W::size &winsize) {
	bgRect->setSz(rct.sz);
}

void LevelView::convertEventCoords(W::Event *ev) {
	ev->pos = convertPixelToGridCoords(ev->pos);
}
W::EventPropagation::T LevelView::scrollEvent(W::Event *ev) {
	using namespace W::EventType;
	int scrollDist = 10;
	if (ev->type == ScreenEdgeLeft)        scroll(-scrollDist, 0);
	else if (ev->type == ScreenEdgeRight)  scroll(scrollDist, 0);
	else if (ev->type == ScreenEdgeTop)    scroll(0, -scrollDist);
	else if (ev->type == ScreenEdgeBottom) scroll(0, scrollDist);
	return W::EventPropagation::ShouldContinue;
}
void LevelView::scroll(int x, int y) {
	_offset.x -= x;
	_offset.y -= y;
		// The offset is added to DrawnObjs’ positions to achieve scrolling.
		// When the offset is zero, we view the top left of the map; it can’t be greater than zero
	if (_offset.x > 0) _offset.x = 0;
	if (_offset.y > 0) _offset.y = 0;
		// The max (i.e. maximally negative) offset is the difference between the pixel size of the map
		// and the view’s actual on-screen size
	int max_offset_x = rct.sz.width - level_width * gridsize;
	int max_offset_y = rct.sz.height - level_height * gridsize;
	if (_offset.x < max_offset_x) _offset.x = max_offset_x;
	if (_offset.y < max_offset_y) _offset.y = max_offset_y;
	
	bgRect->setPos(W::position(-_offset.x, -_offset.y));
}

W::position LevelView::convertGridToPixelCoords(const W::position &_p) {
	return W::position(
		(int)((_p.x + _p.a) * gridsize),
		(int)((_p.y + _p.b) * gridsize)
	);
}
W::size LevelView::convertGridToPixelCoords(const W::size &_s) {
	return W::size(
		_s.width * gridsize,
		_s.height * gridsize
	);
}
W::position LevelView::convertPixelToGridCoords(const W::position &_p) {
	return W::position(
		_p.x/gridsize,
		_p.y/gridsize,
		float((_p.x)%gridsize) / float(gridsize),
		float((_p.y)%gridsize) / float(gridsize)
	);
}
