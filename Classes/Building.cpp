#include "Building.h"

Building::Building() {
	// initialize building
	mode = PLACEMENT;
	std::cout << "created building";
	width = 100, height = 80;
}
Building::~Building()
{
	// Destroy things
}

void Building::handleMouseMove(int x, int y)
{
	posX = x, posY = y;
}

bool Building::placeAt(int x, int y)
{
	mode = PLACED;
	posX = x, posY = y;
	std::cout << "placed building at " << x << ","  << y << std::endl;
	return true;
}

void Building::draw(sf::RenderWindow &wind)
{
	if (mode == PLACEMENT)
		posX = sf::Mouse::GetPosition(wind).x, posY = sf::Mouse::GetPosition(wind).y;
	sf::Shape s = sf::Shape::Rectangle(
		posX - width/2, posY - height/2, width, height,
		(mode == PLACEMENT) ? sf::Color::White : sf::Color::Black
	);
	wind.Draw(s);
}

#ifdef yeah_this_doesnt_need_compiling_either_sorry

  What is a building?
  
  A building is an area displayed at a paticular location on the map.
  When it is created, the user has the chance to specify where it can be built.
  	- some parts of the map may be buildable, others not
  
  A building also defines behaviours of some other objects.
  A staff unit may seek out or be assigned to a building
  
  A building generates its own internal mapping region, also – i.e. they define a free movement zone,
  and form a node in the pathfinding system’s nodelist.
  
  
  Buildings are like other things in the game, like units, obstacles, implements, furniture in
  some key respects – each object must have a sprite, and the code for managing the display of that sprite
  (or returning it for display by some more powerful, parent drawing class.)

#endif