#include "Placeable.hpp"
#include "PlaceableManager.hpp"
#include "LevelState.hpp"
#include "LevelView.hpp"

Placeable::Placeable(PlaceableManager *_mngr) :
	mngr(_mngr)
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
	if (ev->type == LV_MOUSEMOVE) {
		pos = p;
		mngr->placementLoopUpdate();
	}
	else if (ev->type == LV_LEFTMOUSEUP) {
		if (mngr->attemptToPlace(p))
			deactivate();
	}
	else if (ev->type == LV_RIGHTMOUSEUP) {
		deactivate();
		mngr->cancel();
	}
	return W::EventPropagation::SHOULD_STOP;
		// Doesn't really matter which -- placeable only ever receives events
		// while Privileged Event Responder.
}

bool Placeable::activate() {
	using namespace W::EventType;
	W::Callback cb(&Placeable::mouseEvent, this);
	bool success = W::Messenger::requestPrivilegedResponderStatusForEventType(LV_MOUSEMOVE, cb)
		&& W::Messenger::requestPrivilegedResponderStatusForEventType(LV_LEFTMOUSEDOWN, cb)
		&& W::Messenger::requestPrivilegedResponderStatusForEventType(LV_LEFTMOUSEUP, cb)
		&& W::Messenger::requestPrivilegedResponderStatusForEventType(LV_RIGHTMOUSEDOWN, cb)
		&& W::Messenger::requestPrivilegedResponderStatusForEventType(LV_RIGHTMOUSEUP, cb);
	if (!success) deactivate();
	return success;
}
void Placeable::deactivate() {
	using namespace W::EventType;
	W::Messenger::relinquishPrivilegedResponderStatusForEventType(LV_MOUSEMOVE, this);
	W::Messenger::relinquishPrivilegedResponderStatusForEventType(LV_LEFTMOUSEDOWN, this);
	W::Messenger::relinquishPrivilegedResponderStatusForEventType(LV_LEFTMOUSEUP, this);
	W::Messenger::relinquishPrivilegedResponderStatusForEventType(LV_RIGHTMOUSEDOWN, this);
	W::Messenger::relinquishPrivilegedResponderStatusForEventType(LV_RIGHTMOUSEUP, this);
}
