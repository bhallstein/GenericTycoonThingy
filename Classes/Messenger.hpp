/*
 * Messenger.hpp - a static, subscription-based sender of events
 *
 * - To subscribe an EventResponder to an event, use Messenger::subscribe(ptr_to_resp, event_type);
 * - To unsubscribe an EventResponder from an event, use Messenger::unsubscribe(ptr_to_resp, event_type);
 * - When destroying an object that has subscribed to receive events, obviously you must unsubscribe it.
 *    - To make this simple, Messenger provides unsubscribe(ptr_to_resp), which unsubscribes the resp from any and all events.
 *
 */

#ifndef MESSENGER_H
#define MESSENGER_H

#include <map>
#include <list>

#include "Event.hpp"

class EventResponder;

typedef Event::event_type event_type;
typedef std::list<EventResponder *> er_list;
typedef std::map<event_type, er_list> subscription_map;

class Messenger {
public:
	// Methods
	static void subscribe(EventResponder *resp, event_type ev_type) {	// sub a resp to a particular event type
		er_list *subs = &subscriptions[ev_type];
		subs->push_back(resp);
	}
	static void unsubscribe(EventResponder *resp, event_type ev_type) {	// unsub resp from a particular event type
		er_list *subs = &subscriptions[ev_type];
		subs->remove(resp);
	}
	static void unsubscribe(EventResponder *resp) {						// unsub resp from all events
		for (subscription_map::iterator it = subscriptions.begin(); it != subscriptions.end(); it++)
			it->second.remove(resp);
	}
	static void send(Event *);

protected:
	// Properties
	static subscription_map subscriptions;
	
};

#endif
