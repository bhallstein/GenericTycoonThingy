/*
 * View.hpp - a view
 *
 * Subclass to create your own views. Uses JenniferAniston to maintain its positioning information.
 * 
 */

#ifndef VIEW_H
#define VIEW_H

#include <iostream>
#include <list>
#include <typeinfo>

#include "types.hpp"
#include "EventResponder.hpp"
#include "JenniferAniston.hpp"

class Event;

class View : public EventResponder {
public:
	View(W *, JenniferAniston &);
	~View();
	
	virtual void updatePosition();				// Sets x, y, width, height based on current W size
	
	void _draw();								// Wrapper: prepares W for drawing the view, then calls draw()
	virtual void draw();						// Override to implement your subclass’s drawing behaviour
	
	void receiveEvent(Event *);					// Converts event to view’s coordinates, and calls processMouseEvent()
	virtual void processMouseEvent(Event *);	// Override to do something with mouse events
	
protected:
	W *theW;
	JenniferAniston jenniferAniston;
};

#endif
