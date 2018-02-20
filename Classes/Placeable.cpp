/*
 * Generic Tycoon Thingy
 *
 * ================
 *  Placeable.cpp
 * ================
 *
 * Copyright (C) 2012 - Ben Hallstein
 * All rights reserved
 *
 */

#include "Placeable.hpp"
#include "PlaceableManager.hpp"
#include "View__Game.hpp"

Placeable::Placeable(PlaceableManager *_mngr, View__Game *_lv) :
	mngr(_mngr), view__game(_lv)
{
	// hai placeable
}
Placeable::~Placeable()
{
	deactivate();
}

W::EventPropagation::T Placeable::mouseEvent(W::Event *ev) {
	using namespace W::EventType;
	W::v2f p = ev->pos;
  p.a = (int) p.a;
  p.b = (int) p.b;

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
	bool success = W::Messenger::requestPrivilegedEventResponderStatus(view__game, MouseMove, cb)
		&& W::Messenger::requestPrivilegedEventResponderStatus(view__game, LMouseDown, cb)
		&& W::Messenger::requestPrivilegedEventResponderStatus(view__game, LMouseUp, cb)
		&& W::Messenger::requestPrivilegedEventResponderStatus(view__game, RMouseDown, cb)
		&& W::Messenger::requestPrivilegedEventResponderStatus(view__game, RMouseUp, cb);
	if (!success) deactivate();
	return success;
}
void Placeable::deactivate() {
	using namespace W::EventType;
	W::Messenger::relinquishPrivilegedEventResponderStatus(view__game, MouseMove, this);
	W::Messenger::relinquishPrivilegedEventResponderStatus(view__game, LMouseDown, this);
	W::Messenger::relinquishPrivilegedEventResponderStatus(view__game, LMouseUp, this);
	W::Messenger::relinquishPrivilegedEventResponderStatus(view__game, RMouseDown, this);
	W::Messenger::relinquishPrivilegedEventResponderStatus(view__game, RMouseUp, this);
}
