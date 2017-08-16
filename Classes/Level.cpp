#include "Level.h"

Level::Level(int _w, int _h)
{
	w = _w, h = _h;
	
	// Load GameMap
	gamemap.setDimensions(w, h);
}
Level::~Level()
{
	// Destructor
}

Building* Level::createBuilding()
{
	Building b;
	buildings.push_back(b);
	std::cout << "created building: " << buildings.size() << std::endl;
	return &buildings.back();
}


void Level::draw(sf::RenderWindow &window, int block_width, int block_height)
{
	// Draw buildings
	for (std::vector<Building>::iterator i = buildings.begin(); i < buildings.end(); i++) {
		if ((*i).destroyed)
			buildings.erase(i--);
		else {
			window.Draw(
				sf::Shape::Rectangle(
					(*i).x * block_width, (*i).y * block_height, (*i).w * block_width, (*i).h * block_height,
					(*i).col() == 'w' ? sf::Color::White : sf::Color::Black
				)
			);
		}
	}
}


#ifdef here_is_a_small_note_about_drawing_and_coordinates

Two coordinate systems:
	block coordinates: x, y
	floating point coords  (offset from the block): a, b

Level:
	w, h	blocks wide/tall: intcoord
Drawn Object:
	x, y	block coordinate
	[a, b	floating point coordinate within block: floatcoord]

GameMap makes x,y coords available.
Drawn Objects make x,y & a,b coord available.
Level makes all objects currently in play available.

Display code:
	x, y, a, b

Route finding:
	x, y


#endif
