#include "Building.h"

Building::Building(sf::Vector2i _block_size)
{
	mode = PLACEMENT;
	size_in_blocks.x = 6;
	size_in_blocks.y = 4;
	block_size = _block_size;

	// Builing state relevant to the LevelMap.
	destroyed = false;
	position_updated = false;
	prev_position.x = prev_position.y = NO_PREV_POSITION;
	
}
Building::~Building()
{
	// Destroy things
}

void Building::setPositionFromMouse(int x, int y) {
	prev_position.x = pos.x;
	prev_position.y = pos.y;
	
	int remainder_x = x % block_size.x;
	int remainder_y = y % block_size.y;

	int topleftcorner_x = x - (size_in_blocks.x * block_size.x)/2;
	int topleftcorner_y = y - (size_in_blocks.y * block_size.y)/2;
	
	pos.x = topleftcorner_x - remainder_x;
	pos.y = topleftcorner_y - remainder_y;
	
	position_updated = true;
}

void Building::getBounds(int *x1, int *y1, int *x2, int *y2) {
	*x1 = pos.x;
	*y1 = pos.y;
	*x2 = pos.x + size_in_blocks.x * block_size.x;
	*y2 = pos.y + size_in_blocks.y * block_size.y;
}

void Building::receiveEvent(sf::Event *ev, EventResponder **p_e_r) {
	if (mode == PLACEMENT) {
		if (ev->Type == sf::Event::MouseMoved) {
			this->setPositionFromMouse(ev->MouseMove.X, ev->MouseMove.Y);
		}
		else if (ev->Type == sf::Event::MouseButtonPressed) {
			if (ev->MouseButton.Button == sf::Mouse::Left) {
				mode = PLACED;
				*p_e_r = NULL;
			}
			else if (ev->MouseButton.Button == sf::Mouse::Right) {
				destroyed = true;
				*p_e_r = NULL;
			}
		}
	}
	else if (mode == PLACED) {
		
	}
	//std::cout << "Building pos: " << pos.x << "," << pos.y << std::endl;
}

void Building::draw(sf::RenderWindow &w)
{
	w.Draw(
		sf::Shape::Rectangle(
			pos.x, pos.y, (int) size_in_blocks.x * block_size.x, (int) size_in_blocks.y * block_size.y,
			(mode == PLACEMENT) ? sf::Color::White : sf::Color::Black
		)
	);
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
  
  Buildings are like other things in the game (units, obstacles, implements, furniture...) in
  some key respects – for example, each has a sprite, and managed the display of that sprite
  (or returns it for display by some more powerful parent class.)

#endif