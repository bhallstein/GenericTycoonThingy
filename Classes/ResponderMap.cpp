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
}

void ResponderMap::dispatchEvent(Event *ev) {
	// Subscriptions to event types
	if (type_subscriptions.count(ev->type))
		type_subscriptions[ev->type].back()->receiveEvent(ev);
	// Keys
	if (ev->type == Event::KEYPRESS && keycode_subscriptions.count(ev->key))
		keycode_subscriptions[ev->key].back()->receiveEvent(ev);
	// Mouses
	else if (ev->type == Event::MOUSEMOVE || ev->type == Event::LEFTCLICK || ev->type == Event::RIGHTCLICK)
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
	
	/* Old ordering of event passing in the level stuff follows */
	// Get unit whose centre is closest to mouse. Search this & adjacent blocks.
	/*	std::list<MappedObj *> *objs;
	 std::list<MappedObj *>::iterator it;
	 const std::type_info *unit_type = &typeid(Unit);
	 //const std::type_info *vehicle_type = &typeid(Vehicle);
	 float xm = ev->x + ev->a, ym = ev->y + ev->b;
	 float xdist, ydist, dist, closest_unit_dist = INFINITAH;
	 MappedObj *closest_unit = NULL, *u;
	 
	 for (int j = ym-1; j <= ym+1; j++) for (int i = xm-1; i <= xm+1; i++) {
	 if (i < 0 || i >= grid_w || j < 0 || j >= grid_h)
	 continue;
	 objs = &responderMap[j * grid_w + i];
	 for (it = objs->begin(); it != objs->end(); it++)
	 if (typeid(*(u = *it)) == *unit_type) {
	 xdist = xm - (u->x + u->a + 0.5), ydist = ym - (u->y + u->b + 0.5);		// add 0.5 to (x,y) to get centre of unit
	 xdist *= xdist, ydist *= ydist;
	 if (xdist > 0.25 || ydist > 0.25) continue;			// check |xdist| & |ydist| < 0.5
	 dist = xdist*xdist + ydist*ydist;
	 if (dist < closest_unit_dist)
	 closest_unit = u, closest_unit_dist = dist;
	 }
	 }
	 
	 if (closest_unit != NULL) 			closest_unit->receiveEvent(ev);
	 //else if (closest_vehicle != NULL)	closest_vehicle->receiveEvent(ev);
	 
	 // Otherwise send to placeable, and otherwise, building, and otherwise, whatever else is there
	 MappedObj *placeable = NULL, *building = NULL, *button = NULL;
	 const std::type_info *placeable_type = &typeid(Placeable), *building_type = &typeid(Building), *button_type = &typeid(Button);
	 objs = &responderMap[ev->y * grid_w + ev->x];
	 for (it = objs->begin(); it != objs->end(); it++)
	 if (typeid(*(*it)) == *placeable_type)
	 placeable = *it;
	 else if (typeid(*(*it)) == *building_type)
	 building = *it;
	 else if (typeid(*(*it)) == *button_type)
	 button = *it;
	 if (placeable != NULL)		placeable->receiveEvent(ev);
	 else if (building != NULL)	building->receiveEvent(ev);
	 else if (button != NULL)	button->receiveEvent(ev);*/
}
