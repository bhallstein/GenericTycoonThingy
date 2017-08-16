#include "Level.hpp"

Level::Level(int _w, int _h, GameMap *_gamemap)
{
	w = _w, h = _h;
	gamemap = _gamemap;
	framecount = 0;
}
Level::~Level()
{
	// Buildings & units are allocated on the heap with `new` – so must be manually `delete`d.
	for (std::vector<Building*>::iterator i = buildings.begin(); i != buildings.end(); i++)
		delete (*i);	
	for (std::vector<Unit*>::iterator i = units.begin(); i != units.end(); i++)
		delete (*i);
}

Building* Level::createBuilding()
{
	std::cout << "adding new building (currently " << buildings.size() << ")... ";
	buildings.push_back( new Building(gamemap) );
	std::cout << "now: " << buildings.size() << std::endl;
	return buildings.back();
}

Unit* Level::createUnit() {
	std::cout << "adding new unit (currently " << units.size() << ")" << std::endl;
	units.push_back( new Unit(gamemap) );
	return units.back();
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
	for (std::vector<Unit*>::iterator i = units.begin(); i != units.end(); i++) {
		window.Draw(
			sf::Shape::Rectangle(
				(*i)->x * block_width, (*i)->y * block_height, (*i)->w * block_width, (*i)->h * block_height,
				(*i)->col() == 'r' ? sf::Color::Red : (*i)->col() == 'b' ? sf::Color::Black : sf::Color::White
			)
		);
	}
	if (framecount == 1200) framecount = 0;
	if (50 == framecount++) this->createUnit();	// Create a new unit every 20 seconds
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
