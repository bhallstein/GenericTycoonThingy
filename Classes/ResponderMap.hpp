/* 
 * ResponderMap.h - manages event subscriptions
 *
 */

#ifndef RESPONDERMAP_H
#define RESPONDERMAP_H

#include <vector>
#include <list>
#include <map>
#include <iostream>

#include "types.hpp"
#include "Event.hpp"

class EventResponder;
class MappedObj;

typedef Event::event_type event_type;
typedef Event::key_code key_code;

typedef std::list<EventResponder *> er_list;
typedef std::list<MappedObj *> mo_list;

typedef std::map<key_code, er_list> key_subscription_map;
typedef std::map<event_type, er_list> type_subscription_map;

class ResponderMap {
public:
	ResponderMap(int _w, int _h);
	~ResponderMap();
	
	// Methods
	void setSize(int _w, int _h);
	
	// These methods subscribe responders to mouse events that fall over the top of them
	void addResponder(EventResponder *);
	void removeResponder(EventResponder *);
	void addMappedObj(MappedObj *);				// Use these methods for objects with a ground plan
	void removeMappedObj(MappedObj *);			
	// rather than a simple width/height
	
	bool requestPrivilegedEventResponderStatus(EventResponder *);		// lol
	void relinquishPrivilegedEventResponderStatus(EventResponder *);	//
	
	// These are for subscribing responders to key presses
	void subscribeToKey(EventResponder *resp, key_code key);
	void unsubscribeFromKey(EventResponder *resp, key_code key);
	void unsubscribeFromAllKeys(EventResponder *resp);
	
	// These are for subscribing responders to *all* events of a particular type, e.g. screenedge events
	void subscribeToEventType(EventResponder *resp, event_type type);
	void unsubscribeFromEventType(EventResponder *resp, event_type type);
	
	void dispatchEvent(Event *);

protected:
	// Methods
	void dispatchMouseEvent(Event *);
	
	// Properties
	int w, h;
	er_list subscribed_responders;
	mo_list subscribed_mappedobjs;
	key_subscription_map keycode_subscriptions;
	type_subscription_map type_subscriptions;
	
	EventResponder *privileged_event_responder;
};

#endif
