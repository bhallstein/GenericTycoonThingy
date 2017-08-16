#include "Level.hpp"

Level::Level(int _w, int _h, GameMap *_gamemap)
{
	w = _w, h = _h;
	gamemap = _gamemap;
}
Level::~Level()
{
	// Destructor
	for (std::vector<Building*>::iterator i = buildings.begin(); i != buildings.end(); i++)
		delete (*i);	// Buildings are allocated on heap with `new` – so must be manually `delete`d.
}

Building* Level::createBuilding()
{
	std::cout << "adding new building, currently: " << buildings.size() << "... ";
	buildings.push_back( new Building(gamemap) );
	std::cout << "done. now: " << buildings.size() << std::endl;
	return buildings.back();
}


void Level::draw(sf::RenderWindow &window, int block_width, int block_height)
{
	// Draw buildings
	for (std::vector<Building*>::iterator i = buildings.begin(); i < buildings.end(); i++) {
		if ((*i)->destroyed) {
			delete (*i);
			buildings.erase(i--);
		}
		else {
			window.Draw(
				sf::Shape::Rectangle(
					(*i)->x * block_width, (*i)->y * block_height, (*i)->w * block_width, (*i)->h * block_height,
					(*i)->col() == 'w' ? sf::Color::White : (*i)->col() == 'l' ? sf::Color::Blue : sf::Color::Black
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
