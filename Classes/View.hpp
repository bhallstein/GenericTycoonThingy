/*
 * View.h - a view
 *
 * Subclass this to create your own views.
 *
 * Internal coordinates
 *  - takes (blocks_w, blocks_h) params: these allow you to specify a drawing/eventresponse grid
 *  - the purpose of this is simple eventreponse and drawing
 *  - you are not limited to aligning your drawing to this grid, since you also have access to the pixel properties
 *    of the view (l_pos, t_pos, r_pos & b_pos) and events’ floating point offset coords (a & b).
 *
 * Constructor takes:
 *  - a ptr to the window
 *  - two ints: how many blocks to subdivide the view into, vertically & horizontally
 *  - four ints: the offsets from the window’s edges of each of the View’s sides.
 *      - a positive offset of 50 for the left side would set the views’s left edge 50 pixels from the left edge of the window.
 *      - a negative offset of -50 for the left side would set the views’s left edge 50 pixels from the *right* edge of the window.
 * 
 * Drawing functions:
 *  - Currently provides drawRect(colour, x, y, width, height)
 *
 * Event Response functions
 *  - Can be subscribed to an EventHandler, which will pass it mouse events.
 *  - Conversion to block coordinates is automatic. For higher precision, Event has floats ::a and ::b.
 *  - The rationale for this is that for coordinate conversion of events, the pixel width and height of map blocks are required, and
 *    Views are the only thing which should worry about this
 * 
 * To prepare your View for event response:
 *  - Instantiate your View ("myview") with attention to the blocks-wide and -tall parameters
 *  - call `myview.createEventResponderMap()`
 *  - add myview to an EventHandler: `eventHandler.subscribe(&myview);`
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
#include "Unit.hpp"


class View {
public:
	View(sf::RenderWindow *, int _blocks_w = 10, int _blocks_h = 10, int _l_offset = 0, int _t_offset = 0, int _r_offset = 0, int _b_offset = 0);
	~View();
	
	// Methods
	virtual void draw();					// Your subclass should override this

	void acceptEvent(Event *);
	virtual void _acceptEvent(Event *);		// Wrapper for acceptEvent: EventHandler calls this, allowing View & subclasses to do some
											// shit before calling acceptEvent function
	
	void createEventResponderMap(); 		// Create structures necessary for event response
	void addResponder(EventResponder *);
	void removeResponder(EventResponder *);
	
	bool requestPrivilegedEventResponderStatus(EventResponder *);		// lol
	void relinquishPrivilegedEventResponderStatus(EventResponder *);	//
	
	// Properties
	int l_pos, t_pos, r_pos, b_pos;
	int l_offset, t_offset, r_offset, b_offset;

protected:
	// Methods
	virtual void drawRect(sf::Color color, int atX, int atY, int width, int height, float x_offset = 0, float y_offset = 0);
	virtual void drawEventResp(EventResponder *resp, bool floatoffset = false);

	// Properties
	sf::RenderWindow *window;

	bool ready_for_event_response;
	int blocks_w, blocks_h;
	std::vector<std::list<EventResponder*> > responderMap;
	EventResponder *privileged_event_responder;
	
};


class ScrollingView : public View {
public:
	ScrollingView(sf::RenderWindow *, int _blocks_w, int _blocks_h, int _l_offset, int _t_offset, int _r_offset, int _b_offset);
	~ScrollingView();
	
	// Methods
	void _acceptEvent(Event *);
	
protected:
	// Methods
	void drawRect(sf::Color colour, int atX, int atY, int width, int height, float x_offset = 0, float y_offset = 0);	
	
	// Properties
	int block_size_x, block_size_y;		// Subclasses should override. Default is 20.
	int scroll_x, scroll_y;
	
};


#endif
