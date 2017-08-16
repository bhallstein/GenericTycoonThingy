/*
 * View.h - a view
 *
 * Subclass this to create your own views.
 *
 * Internal coordinates
 *  - It’s very convenient to divide a View into a grid of blocks.
 *  - This simplifies both drawing and eventresponse.
 *
 * Offsets from the window edge
 *  - A View’s position within the window is specified using offsets from the edges
 *  - For instance, if you set the left_offset parameter to 50, the left edge of the view will be 50 pixels from the left edge of the window
 *  - Or if you set left_offset to -50, the left edge of the view will be 50 pixels from the *right* edge of the window
 *
 * Constructor takes:
 *  - a pointer to the window
 *  - two ints: how many "blocks" to subdivide the view into, vertically & horizontally
 *  - four ints: the offsets from the window’s edges of each of the View’s sides.
 * 
 * Drawing:
 *  - To implement your subclass’s drawing behaviour, override draw()
 *  - From inside draw(), you can use View’s convenience drawing function(s):
 *  	- drawRect(colour, x, y, width, height)
 * 			- (takes block coordinates)
 *  - You’re not limited to drawing in the block coordinate system, though. You can also draw directly to the window using pixel values.
 *
 * Event Response functions
 *  - Can be subscribed to an EventHandler, which will pass it mouse events.
 *  - Can use View::addResponder() & removeResponder() to objects for mouse events
 * 
 * To prepare your View for event response:
 *  - Create the instance
 *  - Subscribe it to an EventHandler: `eh.subscribe(&myview);`
 * 
 */

#ifndef VIEW_H
#define VIEW_H

#include <iostream>
#include <list>
#include <typeinfo>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "types.h"
#include "Event.hpp"
#include "EventResponder.hpp"
#include "MappedObj.hpp"


class View {
public:
	View(sf::RenderWindow *, int _blocks_w = 10, int _blocks_h = 10, int _l_offset = 0, int _t_offset = 0, int _r_offset = 0, int _b_offset = 0);
	~View();
	
	// Methods
	virtual void draw();					// Your subclass should override this

	void dispatchMouseEvent(Event *);
	virtual void _dispatchMouseEvent(Event *);	// EventHandler calls this, allowing View & subclasses to do some
												// shit before calling dispatchMouseEvent function
	
	void addResponder(MappedObj *);
	void removeResponder(MappedObj *);
	
	bool requestPrivilegedEventResponderStatus(EventResponder *);		// lol
	void relinquishPrivilegedEventResponderStatus(EventResponder *);	//
	
	// Properties
	int l_pos, t_pos, r_pos, b_pos;
	int l_offset, t_offset, r_offset, b_offset;

protected:
	// Methods
	virtual void drawRect(sf::Color color, int atX, int atY, int width, int height, float x_offset = 0, float y_offset = 0);
	virtual void drawMappedObj(MappedObj *);
	
	// Properties
	sf::RenderWindow *window;
	
	int blocks_w, blocks_h;
	std::vector<std::list<MappedObj *> > responderMap;
	EventResponder *privileged_event_responder;
	
};


class ScrollingView : public View {
public:
	ScrollingView(sf::RenderWindow *, int _blocks_w, int _blocks_h, int _l_offset, int _t_offset, int _r_offset, int _b_offset);
	~ScrollingView();
	
	// Methods
	void _dispatchMouseEvent(Event *);
	
protected:
	// Methods
	void drawRect(sf::Color colour, int atX, int atY, int width, int height, float x_offset = 0, float y_offset = 0);	
	
	// Properties
	int block_size_x, block_size_y;		// Subclasses should override. Default is 20.
	int scroll_x, scroll_y;
	
};


#endif
