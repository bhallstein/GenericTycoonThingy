/*
 * View.h - a view
 *
 * Subclass this to create your own views.
 *
 * Internal coordinates
 *  - Because it’s convenient, we divide a View into a grid of blocks.
 *  - This simplifies both drawing and event response.
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
 *		- drawMappedObj(MappedObj *)
 * 			- draw an object. uses the object’s own position info
 *  	- drawRect(colour, x, y, width, height)
 * 			- (takes block coordinates)
 *  - You’re not limited to drawing in the block coordinate system, though. You can also draw directly to the window using pixel values.
 *
 * Event Response functions
 *  - Can be subscribed to an EventHandler, which will pass it mouse events.
 *  - Can use View::addResponder() & removeResponder() to objects for mouse events
 *  - Events are then passed automatically to the appropriate responders, with pixel precision.
 *  - If your subclass does something wacky, you may like to override convertEventToBlockCoordinates().
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
	View(sf::RenderWindow *, int _grid_w = 10, int _grid_h = 10, int _offsetL = 0, int _offsetT = 0, int _offsetR = 0, int _offsetB = 0);
	~View();
	
	// Methods
	virtual void draw();					// Override this to implement your subclass’s drawing behaviour
	
	void addResponder(MappedObj *);
	void removeResponder(MappedObj *);
	void dispatchMouseEvent(Event *);
	
	bool requestPrivilegedEventResponderStatus(EventResponder *);		// lol
	void relinquishPrivilegedEventResponderStatus(EventResponder *);	//
	
	// Properties
	int l_pos, t_pos, r_pos, b_pos;
	int offset_L, offset_T, offset_R, offset_B;

protected:
	// Methods
	virtual void drawRect(sf::Color, int atX, int atY, int width, int height, float x_offset = 0, float y_offset = 0);
	virtual void drawMappedObj(MappedObj *);
	virtual void convertEventToBlockCoordinates(Event *ev);		// Convert an event’s coords (pixels, rel. to window) to coords in the internal grid
																// (override if subclass does something non-standard with the grid)
	// Properties
	sf::RenderWindow *window;
	int grid_w, grid_h;

	std::vector<std::list<MappedObj *> > responderMap;
	EventResponder *privileged_event_responder;
	
};


class ScrollingView : public View {
public:
	ScrollingView(sf::RenderWindow *, int _blocks_w, int _blocks_h, int _l_offset, int _t_offset, int _r_offset, int _b_offset);
	~ScrollingView();
	
protected:
	// Methods
	void drawRect(sf::Color, int _x, int _y, int _w, int _h, float x_offset = 0, float y_offset = 0);
	void convertEventToBlockCoordinates(Event *ev);
	
	// Properties
	int block_pixel_width, block_pixel_height;		// Width/height of a "block". Default is 20.
	int scroll_x, scroll_y;
	
};


#endif
