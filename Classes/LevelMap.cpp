#include "LevelMap.h"

/*********************************
 *   MapLocation implementation  *
 *********************************/

MapLocation::MapLocation() {
	// Constructor
}
MapLocation::~MapLocation() {
	// Destructor
}

void MapLocation::addBuilding(Building *b) {
	// Add a building to the list thereof
	buildings.push_back(b);
}
void MapLocation::removeBuilding(Building *b) {
	// Remove a bulding from the list thereof
	buildings.remove(b);
}



/*********************************
 *    LevelMap implementation    *
 *********************************/

LevelMap::LevelMap(int _rows, int _columns, int _width, int _height)
{
	rows = rows, columns = _columns;
	block_size.x = _width/_columns;
	block_size.y = _height/_rows;
	
	maplocs.resize(block_size.x * block_size.y);
}
LevelMap::~LevelMap()
{
	// Destructor
}

int LevelMap::numberOfBuildings() {
	return buildings.size();
}

Building* LevelMap::createBuilding()
{
	Building b(block_size);
	buildings.push_back(b);
	std::cout << "created building: " << buildings.size() << std::endl;
	return &buildings.back();
}

void LevelMap::destroyBuilding()
{
	buildings.pop_back();
	std::cout << "cancelled building creation. " << std::endl;
}

void LevelMap::moveBuilding(Building *b, int x1, int y1, int x2, int y2)
{
	//// Remove from current maploc
	//if (x1 >= 0 and y1 >= 0)
	//	(*maplocs[y1 * rows + x1]).removeBuilding(b);
	//	
	//// Add to new maploc
	//(*maplocs[y2 * rows + x2]).addBuilding(b);
}


void LevelMap::draw(sf::RenderWindow &window)
{
	// Draw buildings
	for (vector<Building>::iterator i = buildings.begin(); i < buildings.end(); i++)
		(*i).draw(window);
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