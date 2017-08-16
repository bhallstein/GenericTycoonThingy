#include "EventHandler.hpp"

EventHandler::EventHandler() {
	
}
EventHandler::~EventHandler() {
	
}


void EventHandler::subscribe(View *v) {
	unsubscribe(v);
	subscribed_views.push_back(v);
	std::cout << "View " << v << " subscribed to receive mouse events" << std::endl;
}
void EventHandler::unsubscribe(View *v) {
	subscribed_views.remove(v);
}

void EventHandler::subscribe(EventResponder* resp, dbt_event_code key_code) {
	keycode_subscribed_responders[key_code].remove(resp);
	keycode_subscribed_responders[key_code].push_back(resp);
}
void EventHandler::unsubscribe(EventResponder* resp, dbt_event_code key_code) {
	if (!keycode_subscribed_responders.count(key_code)) return;
	keycode_subscribed_responders[key_code].remove(resp);
}

void EventHandler::dispatchEvent(Event *ev) {
	View *view;

	// Keys
	if (ev->type == KEYPRESS && keycode_subscribed_responders.count(ev->key))
		// Send to responders subscribed to that key
		for (std::list<EventResponder*>::iterator i = keycode_subscribed_responders[ev->key].begin(); i != keycode_subscribed_responders[ev->key].end(); i++)
			(*i)->receiveEvent(ev);
	
	// Mouses
	else if (ev->type == MOUSEMOVE || ev->type == LEFTCLICK || ev->type == RIGHTCLICK)
		// Send to subscribed views that intersect with the event.
		for (std::list<View*>::iterator i = subscribed_views.begin(); i != subscribed_views.end(); i++) {
			view = (*i);
			if (ev->x >= view->l_pos && ev->x <= view->r_pos && ev->y >= view->t_pos && ev->y <= view->b_pos)
				view->dispatchMouseEvent(ev);
		}
}
