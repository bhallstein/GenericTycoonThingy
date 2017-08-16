#include "Level.h"

Level::Level(int _columns, int _rows, int _width, int _height)
{
	rows = _rows, columns = _columns;
	block_size.x = _width/_columns;
	block_size.y = _height/_rows;
}
Level::~Level()
{
	// Destructor
}

Building* Level::createBuilding()
{
	Building b(block_size);
	buildings.push_back(b);
	std::cout << "created building: " << buildings.size() << std::endl;
	return &buildings.back();
}


void Level::draw(sf::RenderWindow &window)
{
	// Draw buildings
	for (vector<Building>::iterator i = buildings.begin(); i < buildings.end(); i++) {
		if ((*i).destroyed)
			buildings.erase(i--);
		else
			(*i).draw(window);
	}
}
