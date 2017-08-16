#include "ResponderMap.hpp"
#include "EventResponder.hpp"
#include "MappedObj.hpp"

ResponderMap::ResponderMap(int _w, int _h) : w(_w), h(_h), privileged_event_responder(NULL) {
	
}
ResponderMap::~ResponderMap() {
	
}

void ResponderMap::setSize(int _w, int _h) {
	w = _w, h = _h;
}

void ResponderMap::addResponder(EventResponder *resp) {
	removeResponder(resp);
	subscribed_responders.push_back(resp);
}
void ResponderMap::removeResponder(EventResponder *resp) {
	subscribed_responders.remove(resp);
}

void ResponderMap::addMappedObj(MappedObj *obj) {
	removeMappedObj(obj);
	subscribed_mappedobjs.push_back(obj);
}
void ResponderMap::removeMappedObj(MappedObj *obj) {
	subscribed_mappedobjs.remove(obj);
}

bool ResponderMap::requestPrivilegedEventResponderStatus(EventResponder *resp) {
	if (privileged_event_responder != NULL) return false;
	privileged_event_responder = resp;
	return true;
}
void ResponderMap::relinquishPrivilegedEventResponderStatus(EventResponder *resp) {
	if (privileged_event_responder == resp)
		privileged_event_responder = NULL;
}

void ResponderMap::subscribeToKey(EventResponder *resp, key_code key) {
	keycode_subscriptions[key].remove(resp);
	keycode_subscriptions[key].push_back(resp);
}
void ResponderMap::unsubscribeFromKey(EventResponder *resp, key_code key) {
	if (!keycode_subscriptions.count(key)) return;
	keycode_subscriptions[key].remove(resp);
}
void ResponderMap::unsubscribeFromAllKeys(EventResponder *resp) {
	for (key_subscription_map::iterator it = keycode_subscriptions.begin(); it != keycode_subscriptions.end(); it++)
		it->second.remove(resp);
}

void ResponderMap::subscribeToEventType(EventResponder *resp, event_type type) {
	type_subscriptions[type].remove(resp);
	type_subscriptions[type].push_back(resp);
}
void ResponderMap::unsubscribeFromEventType(EventResponder *resp, event_type type) {
	if (!type_subscriptions.count(type)) return;
	type_subscriptions[type].remove(resp);
	if (type_subscriptions[type].size() == 0)
		type_subscriptions.erase(type);
}

void ResponderMap::dispatchEvent(Event *ev) {
	// Subscriptions to event types
	if (type_subscriptions.count(ev->type))
		type_subscriptions[ev->type].back()->receiveEvent(ev);
	// Keys
	if (ev->type == Event::KEYPRESS && keycode_subscriptions.count(ev->key))
		keycode_subscriptions[ev->key].back()->receiveEvent(ev);
	// Mouses
	else if (ev->type == Event::MOUSEMOVE || ev->type == Event::LEFTCLICK || ev->type == Event::RIGHTCLICK || ev->type == Event::LEFTMOUSEDOWN)
		dispatchMouseEvent(ev);
}

void ResponderMap::dispatchMouseEvent(Event *ev) {
	if (ev->x < 0 || ev->y < 0 || ev->x >= w || ev->y >= h)
		return;
	
	if (privileged_event_responder != NULL) {
		privileged_event_responder->receiveEvent(ev);
		return;
	}
	
	// Find last MappedObj that overlaps the event’s location
	MappedObj *last_obj = NULL, *o;
	for (mo_list::iterator it = subscribed_mappedobjs.begin(); it != subscribed_mappedobjs.end(); it++) {
		o = *it;
		for (int i=0; i < o->groundplan.size(); i++) {
			intcoord c = o->groundplan[i];
			c.x += o->x, c.y += o->y;
			floatcoord c_ev = { ev->x + ev->a, ev->y + ev->b }, c_obj = { c.x + o->a, c.y + o->b };
			if (c_ev.a >= c_obj.a && c_ev.b >= c_obj.b && c_ev.a <= c_obj.a + 1 && c_ev.b <= c_obj.b + 1)
				last_obj = o;
		}
	}
	if (last_obj != NULL) {
		last_obj->receiveEvent(ev);
		return;
	}
	
	// Otherwise, find last ER that overlaps it
	EventResponder *last_resp = NULL, *r;
	for (er_list::iterator it = subscribed_responders.begin(); it != subscribed_responders.end(); it++) {
		r = *it;
		if (r->x <= ev->x && r->x + r->width > ev->x && r->y <= ev->y && r->y + r->height > ev->y)
			last_resp = r;
	}
	if (last_resp != NULL) {
		last_resp->receiveEvent(ev);
		return;
	}
}
