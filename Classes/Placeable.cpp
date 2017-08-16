#include "Placeable.hpp"
#include "PlaceableManager.hpp"
#include "Level.hpp"

Placeable::Placeable(PlaceableManager *_mngr, W::EventHandler *_eh) :
	mngr(_mngr), eh(_eh)
{
	plan.resize(1);
	W::rect *r = &plan[0];
	r->pos.x = r->pos.y = 0;
	r->sz.width = r->sz.height = 1;
}
Placeable::~Placeable()
{
	deactivate();
}

void Placeable::receiveEvent(W::Event *ev) {
	using namespace W::EventType;
	if (ev->type == LEVEL_MOUSEMOVE)
		pos.x = ev->pos.x, pos.y = ev->pos.y;
	else if (ev->type == LEVEL_LEFTMOUSEDOWN) {
		if (!mngr->attemptToPlace(ev->pos.x, ev->pos.y))
			return;
		else
			deactivate();
	}
	else if (ev->type == LEVEL_RIGHTMOUSEDOWN)
		mngr->destroyed = true;
}

bool Placeable::activate() {
	using namespace W::EventType;
	eh->requestPrivilegedResponderStatusForEventType(LEVEL_MOUSEMOVE,      W::Callback(&Placeable::receiveEvent, this));
	eh->requestPrivilegedResponderStatusForEventType(LEVEL_LEFTMOUSEDOWN,  W::Callback(&Placeable::receiveEvent, this));
	eh->requestPrivilegedResponderStatusForEventType(LEVEL_RIGHTMOUSEDOWN, W::Callback(&Placeable::receiveEvent, this));
	return true;
}
void Placeable::deactivate() {
	using namespace W::EventType;
	eh->relinquishPrivilegedResponderStatusForEventType(LEVEL_MOUSEMOVE, this);
	eh->relinquishPrivilegedResponderStatusForEventType(LEVEL_LEFTMOUSEDOWN, this);
	eh->relinquishPrivilegedResponderStatusForEventType(LEVEL_RIGHTMOUSEDOWN, this);
}
