#include "Placeable.hpp"
#include "MappedObj.hpp"

Placeable::Placeable(MappedObj *_mo, ResponderMap *_rm) : mo(_mo), rm(_rm) {
	if (!rm->requestPrivilegedEventResponderStatus(this))
		throw MsgException("Placeable couldn't get privileged event responder status.");
}
Placeable::~Placeable()
{
	rm->relinquishPrivilegedEventResponderStatus(this);
}

void Placeable::receiveEvent(Event *ev) {
	if (ev->type == Event::MOUSEMOVE)
		x = ev->x, y = ev->y;
	else if (ev->type == Event::LEFTCLICK) {
		if (!mo->attemptToPlace(ev->x, ev->y))
			return;
		else
			rm->relinquishPrivilegedEventResponderStatus(this);
	}
	else if (ev->type == Event::RIGHTCLICK)
		mo->destroyed = true;
}
