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

LevelMap::LevelMap(int _columns, int _rows, int _width, int _height)
{
	rows = _rows, columns = _columns;
	block_size.x = _width/_columns;
	block_size.y = _height/_rows;
	
	maplocs.resize(block_size.x * block_size.y);
}
LevelMap::~LevelMap()
{
	// Destructor
	maplocs.clear();
}

Building* LevelMap::createBuilding()
{
	Building b(block_size);
	buildings.push_back(b);
	std::cout << "created building: " << buildings.size() << std::endl;

	Building *new_building = &buildings.back();

	return new_building;
}

//void LevelMap::memmap_addObject(DrawnObject *object) {
//	// Add to memory map
//}
//void LevelMap::memmap_removeObject(DrawnObject *object) {
//	// Remove from memory map
//	// Should a drawn object always implement a prev_position, with the option of NO_PREV_POSITION?
//	// This way, can remove it from memory map without passing x1, y1, x2, y2
//	// Yes, then.
//}

void LevelMap::draw(sf::RenderWindow &window)
{
	// Draw buildings
	for (vector<Building>::iterator i = buildings.begin(); i < buildings.end(); i++) {
		if ((*i).destroyed) {
			// Remove from memory map
			// this->memmap_removeObject(buildings[i]);
			buildings.erase(i--);
		}
		//else if ((*i).position_updated) {
		//	// Move in memory map
		//	this->memmap_removeObject(buildings[i]);
		//	this->memmap_addObject(buildings[i]);
		//	(*i).position_updated = false;
		//	//int x1, y1, x2, y2;
		//	//new_building->getBounds(&x1, &y1, &x2, &y2);
		//	//this->memmap_addObject(new_building, x1, y1, x2, y2);
		//}
		else
			(*i).draw(window);
	}
}
