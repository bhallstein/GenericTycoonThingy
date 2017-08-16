/*
 * View.h - a view
 *
 * Subclass this to create your own views.
 *
 * Constructor takes a ptr to the window, and four ints: the offsets from the window’s edges of each of the View’s sides.
 * A positive offset of 50 for the left side that the left side of the view will be 50 pixels from the left edge of the window.
 * A negative offset of -50 for the left side would make the left side 50 pixels from the *right* edge of the window.
 * 
 * Drawing functions:
 * Currently provides drawRect(colour, x, y, width, height)
 *
 * Event Response functions
 * Can be subscribed to an EventHandler, which will pass it mouse events.
 * The rationale for this is that for coordinate conversion of events, the pixel width and height of map blocks are required, and
 * only the View should have access to these.
 * 
 * To prepare your View for event response:
 *   1. call View::createEventResponderMap()
 *   2. optionally override responder_block_width/height in your subclass.
 *        (the default is 1: a responder list will be created for every pixel.)
 *   3. add your View to an EventHandler using EventHandler::subscribe(&myview);
 *
 * Internal coordinate support
 * If you set your responder_block_width/height to a value other than 1, View will automatically convert Events’ coordinates
 * to your responder coordinates.
 * 
 */

#ifndef VIEW_H
#define VIEW_H

#include <iostream>
#include <list>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "types.h"
#include "Event.hpp"
#include "EventResponder.hpp"


class View {
public:
	View(sf::RenderWindow *, int _l_offset, int _t_offset, int _r_offset, int _b_offset);
	~View();
	
	// Methods
	virtual void draw();					// Your subclass should override this
	virtual void acceptEvent(Event *);		// and this, if you want it to receive events
	
	void _acceptEvent(Event *);
	
	void createEventResponderMap(); 		// Create structures necessary for event response
	void addResponder(EventResponder *);
	void removeResponder(EventResponder *);
	
	bool requestPrivilegedEventResponderStatus(EventResponder *);		// lol
	void relinquishPrivilegedEventResponderStatus(EventResponder *);
	
	// Properties
	int l_pos, t_pos, r_pos, b_pos;
	int l_offset, t_offset, r_offset, b_offset;
	
	int responder_block_width;			// Override these with the pixel size of your
	int responder_block_height;			// response block (if not equal to 1).

protected:
	
	sf::RenderWindow *window;
	void drawRect(sf::Color color, int atX, int atY, int width, int height);

	int responderMap_w, responderMap_h;
	std::vector<std::list<EventResponder*> > responderMap;
	EventResponder *privileged_event_responder;
	
	bool setup_for_event_response;
	
};


#endif
