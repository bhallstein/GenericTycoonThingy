#include "GameMap.h"

/*********************************
 *   MapLocation implementation  *
 *********************************/

// MapLocation::MapLocation() {
// 	// Constructor
// }
// MapLocation::~MapLocation() {
// 	// Destructor
// }
// 
// void MapLocation::addBuilding(Building *b) {
// 	// Add a building to the list thereof
// 	buildings.push_back(b);
// }
// void MapLocation::removeBuilding(Building *b) {
// 	// Remove a bulding from the list thereof
// 	buildings.remove(b);
// }



/*********************************
 *    GameMap implementation    *
 *********************************/

GameMap::GameMap(int columns, int rows)
{
	w = columns, h = rows;
	//maplocs.resize(block_size.x * block_size.y);
}
GameMap::~GameMap()
{
	// Destructor
	//maplocs.clear();
}

//void GameMap::memmap_addObject(DrawnObject *object) {
//	// Add to memory map
//}
//void GameMap::memmap_removeObject(DrawnObject *object) {
//	// Remove from memory map
//	// Should a drawn object always implement a prev_position, with the option of NO_PREV_POSITION?
//	// This way, can remove it from memory map without passing x1, y1, x2, y2
//	// Yes, then.
//}

/* Code taken from previous LevelMap draw function: */
		//else if ((*i).position_updated) {
		//	// Move in memory map
		//	this->memmap_removeObject(buildings[i]);
		//	this->memmap_addObject(buildings[i]);
		//	(*i).position_updated = false;
		//	//int x1, y1, x2, y2;
		//	//new_building->getBounds(&x1, &y1, &x2, &y2);
		//	//this->memmap_addObject(new_building, x1, y1, x2, y2);
		//}