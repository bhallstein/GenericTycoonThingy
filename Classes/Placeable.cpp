#include "Placeable.hpp"
#include "MappedObj.hpp"
#include "../W.hpp"

Placeable::Placeable(MappedObj *_mo, ResponderMap *_rm) : mo(_mo), rm(_rm)
{
	
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
			deactivate();
	}
	else if (ev->type == Event::RIGHTCLICK)
		mo->destroyed = true;
}

bool Placeable::activate() {
	return rm->requestPrivilegedEventResponderStatus(this);
}
void Placeable::deactivate() {
	rm->relinquishPrivilegedEventResponderStatus(this);
}
