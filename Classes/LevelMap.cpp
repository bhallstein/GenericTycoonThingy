#include "LevelMap.h"

LevelMap::LevelMap(int w, int h)
{
	Rows = h;
	Columns = w;
}

int numberOfBuildings() {
	return Buildings.size();
}

void Draw(sf::RenderWindow &window)
{
	// Draw background
	window.Clear(sf::Color::white);
	
	// Draw buildings
	vector<int>::iterator i;
	for (i = Buildings.begin(); i < Buildings.end(); i++)
		; // draw building
}




#ifdef dont_compile_this_pls_its_not_actually_code

  Process for adding a building:
  	- create building object
  		- initialize in mode 'placement':
  			- responds to all mouse events, and blocks other things from responding to them (captures? them) *
  			- on hover, if valid placement, color => green, otherwise red
  			- on click, if valid placement, building changes to mode to 'placed'
  
  * The concept of privileged event responders is required: some things must interrupt the usual mapping of events
    to objects via the map, and be the sole recipient of such events.

	This can be achieved if, when the game loop elicits the creation of a new building, it receives back a reference
	to the building created. The game loop has a privileged_mouse_event_responder, and, before sending events
	down the usual channels, test this, and if it is set, instead sends the mouse events only to that privileged responder.

#endif