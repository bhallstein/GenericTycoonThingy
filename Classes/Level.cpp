#include "Level.hpp"

Level::Level(std::string fileName, GameMap *_gamemap) : gamemap(_gamemap)
{
	buildLevel(readLevel(fileName));	// Read in from level file
	framecount = 0;
}
Level::~Level()
{
	// Buildings & units are allocated on the heap with `new` – so must be manually `delete`d.
	for (std::vector<Building*>::iterator i = buildings.begin(); i != buildings.end(); i++)
		delete (*i);
	for (std::vector<Placeable*>::iterator i = placeables.begin(); i != placeables.end(); i++)
		delete (*i);
	for (std::vector<Unit*>::iterator i = units.begin(); i != units.end(); i++)
		delete (*i);
}

Placeable* Level::createPlaceable()
{
	placeables.push_back( new Placeable(gamemap) );
	std::cout << "added new placeable (now " << placeables.size() << ")" << std::endl;
	return placeables.back();
}

Unit* Level::createUnit() {
	units.push_back( new Unit(gamemap) );
	std::cout << "added new unit (now " << units.size() << ")" << std::endl;
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
	// Draw placeables
	for (std::vector<Placeable*>::iterator i = placeables.begin(); i < placeables.end(); i++) {
		if ((*i)->destroyed) {
			delete (*i);
			placeables.erase(i--);
		}
		else {
			window.Draw(
				sf::Shape::Rectangle(
					(*i)->x * block_width, (*i)->y * block_height, (*i)->w * block_width, (*i)->h * block_height,
					(*i)->col() == 'w' ? sf::Color::White : (*i)->col() == 'r' ? sf::Color::Red : sf::Color::Yellow
				)
			);
		}
	}
	//Draw units
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

//Stuff about loading levels

boost::property_tree::ptree Level::readLevel(std::string fileName) //This read may be replaced by more centralised serialisation later
{
	boost::property_tree::ptree pt;
	read_xml(fileName, pt);

	return pt;
}

void Level::buildLevel(boost::property_tree::ptree levelFile)
{
	//Get width and height from attributes of 'level'
	w = 50; //levelFile.get<int>("<xmlattr>.width");
	h = 40; //levelFile.get<int>("<xmlattr>.height");

	gamemap->setDimensions(w,h);

	BOOST_FOREACH(boost::property_tree::ptree::value_type &obj,levelFile.get_child("level"))
	{
		//type checks
		//if(obj.first == "width") w = boost::lexical_cast<int>(obj.first);
		//if(obj.first == "width") w = boost::lexical_cast<int>(obj.first);

		if(obj.first == "building")
		{
			//Create this building (add it to level.buildings)
			buildings.push_back( new Building(gamemap) );

			Building *b = buildings.back();

			//place it at its block co-ords
			b->setPosition(obj.second.get<int>("x",0),obj.second.get<int>("y",0));
		}
	}
}

#ifdef here_is_a_small_note_about_drawing_and_coordinates_why_here_you_ask_ill_get_back_to_you_on_that_one

Two coordinate systems:
	block coordinates: x, y
	floating point coords  (offset from the block): a, b

Display code:
	x, y, a, b

Route finding:
	x, y

#endif
