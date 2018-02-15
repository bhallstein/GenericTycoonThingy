/*
 * Generic Tycoon Thingy
 *
 * =================
 *  Placeable.cpp
 * =================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#include "Placeable.hpp"
#include "PlaceableManager.hpp"
#include "LevelView.hpp"

Placeable::Placeable(PlaceableManager *_mngr, LevelView *_lv) :
	mngr(_mngr), levelView(_lv)
{
	// hai placeable
}
Placeable::~Placeable()
{
	deactivate();
}

W::EventPropagation::T Placeable::mouseEvent(W::Event *ev) {
	using namespace W::EventType;
	W::position p = ev->pos;
	p.a = p.b = 0;
	if (ev->type == MouseMove) {
		pos = p;
		mngr->placementLoopUpdate();
	}
	else if (ev->type == LMouseUp) {
		if (mngr->attemptToPlace(p))
			deactivate();
	}
	else if (ev->type == RMouseUp) {
		deactivate();
		mngr->cancel();
	}
	return W::EventPropagation::ShouldStop;
		// Doesn't really matter which -- placeable only ever receives events
		// while Privileged Event Responder.
}

bool Placeable::activate() {
	using namespace W::EventType;
	W::Callback cb(&Placeable::mouseEvent, this);
	bool success = W::Messenger::requestPrivilegedEventResponderStatus(levelView, MouseMove, cb)
		&& W::Messenger::requestPrivilegedEventResponderStatus(levelView, LMouseDown, cb)
		&& W::Messenger::requestPrivilegedEventResponderStatus(levelView, LMouseUp, cb)
		&& W::Messenger::requestPrivilegedEventResponderStatus(levelView, RMouseDown, cb)
		&& W::Messenger::requestPrivilegedEventResponderStatus(levelView, RMouseUp, cb);
	if (!success) deactivate();
	return success;
}
void Placeable::deactivate() {
	using namespace W::EventType;
	W::Messenger::relinquishPrivilegedEventResponderStatus(levelView, MouseMove, this);
	W::Messenger::relinquishPrivilegedEventResponderStatus(levelView, LMouseDown, this);
	W::Messenger::relinquishPrivilegedEventResponderStatus(levelView, LMouseUp, this);
	W::Messenger::relinquishPrivilegedEventResponderStatus(levelView, RMouseDown, this);
	W::Messenger::relinquishPrivilegedEventResponderStatus(levelView, RMouseUp, this);
}
