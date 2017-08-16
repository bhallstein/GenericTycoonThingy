#include "Level.hpp"

Level::Level(std::string fileName, sf::RenderWindow *_window, EventHandler *_eventHandler) : window(_window), eventHandler(_eventHandler), 	levelview(_window, 0, 0, 0, 80), uiview(_window, 0, -80, 0, 0)
{
	// Give levelview responderMap powers.
	levelview.createEventResponderMap();
	eventHandler->subscribe(&levelview);
	
	// Build level
	std::cout << "calling buildLevel" << std::endl;
	buildLevel(readLevel(fileName));
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
	delete gamemap;
}

Unit* Level::createUnit() {
	Unit *u = new Unit(gamemap);
	units.push_back(u);
	std::cout << "added new unit (now " << units.size() << ")" << std::endl;
	return u;
}
Building* Level::createBuilding(int atX, int atY) {
	Building *b = new Building(gamemap, eventHandler, atX, atY);
	buildings.push_back(b);
	levelview.addResponder(b);
	eventHandler->subscribe(b, K_L);
	std::cout << "added building " << b << "(now " << buildings.size() << ")" << std::endl;
	return b;
}
void Level::createPlaceable() {
	Placeable *p = new Placeable(gamemap, &levelview);
	if (!levelview.requestPrivilegedEventResponderStatus(p)) {
		delete p;
		return;
	}
	placeables.push_back(p);
	std::cout << "added placeable " << p << " (now " << placeables.size() << ")" << std::endl;
	return;
}

void Level::draw()
{
	levelview.draw(buildings, placeables, units);
	uiview.draw();
	//if (framecount == 1200) framecount = 0;
	//if (50 == framecount++) this->createUnit();	// Create a new unit every 20 seconds
}

ptree Level::readLevel(std::string fileName) //This read may be replaced by more centralised serialisation later
{
	ptree pt;
	read_xml(fileName, pt);
	return pt;
}
void Level::buildLevel(ptree levelFile)
{
	w = levelFile.get<int>("level.<xmlattr>.width");
	h = levelFile.get<int>("level.<xmlattr>.height");
	
	gamemap = new GameMap(w, h);
	
	std::cout << "level width: " << w << std::endl;
	std::cout << "level height: " << h << std::endl;

	std::cout << "creating buildings" << std::endl;
	BOOST_FOREACH(ptree::value_type &obj, levelFile.get_child("level"))
	{
		if(obj.first == "building")
			createBuilding(obj.second.get<int>("x",0), obj.second.get<int>("y",0));
	}
	std::cout << "built level from filey" << std::endl;
}


LevelView::LevelView(sf::RenderWindow *_window, int _l_offset = 0, int _t_offset = 0, int _r_offset = 0, int _b_offset = 0) :
	View(_window, _l_offset, _t_offset, _r_offset, _b_offset)
{
	responder_block_height = 16;
	responder_block_width = 16;
}

void LevelView::acceptEvent(Event *ev) {	
	if (privileged_event_responder != NULL) {
		privileged_event_responder->receiveEvent(ev);
		return;
	}
	
	std::list<EventResponder*> *resps = &responderMap[ev->y * responderMap_w + ev->x];
	for (std::list<EventResponder*>::iterator i = resps->begin(); i != resps->end(); i++)
		(*i)->receiveEvent(ev);
}

void LevelView::draw(std::vector<Building*> buildings, std::vector<Placeable*> placeables, std::vector<Unit*> units) {

	// This is obviously a horrendous way to get info into the view, but the separation of View is nonetheless a structural
	// improvement. The sane way to do would be to pass the game map, of course, which we should do at some point.

	int block_width = responder_block_width, block_height = responder_block_height;

	// Draw buildings
	for (std::vector<Building*>::iterator i = buildings.begin(); i < buildings.end(); i++) {
		if ((*i)->destroyed) {
			delete (*i);
			buildings.erase(i--);
		}
		else
			drawRect(
				(*i)->col() == 'w' ? sf::Color::White : (*i)->col() == 'l' ? sf::Color::Blue : sf::Color::Black,
				(*i)->x * block_width, (*i)->y * block_height, (*i)->w * block_width, (*i)->h * block_height
			);
	}
	// Draw placeables
	for (std::vector<Placeable*>::iterator i = placeables.begin(); i < placeables.end(); i++) {
		if ((*i)->destroyed) {
			delete (*i);
			placeables.erase(i--);
		}
		else
			drawRect(
				(*i)->col() == 'w' ? sf::Color::White : (*i)->col() == 'r' ? sf::Color::Red : sf::Color::Yellow,
				(*i)->x * block_width, (*i)->y * block_height, (*i)->w * block_width, (*i)->h * block_height
			);
	}
	//Draw units
	for (std::vector<Unit*>::iterator i = units.begin(); i != units.end(); i++) {
		drawRect(
			(*i)->col() == 'r' ? sf::Color::Red : (*i)->col() == 'b' ? sf::Color::Black : sf::Color::White,
			(*i)->x * block_width, (*i)->y * block_height, (*i)->w * block_width, (*i)->h * block_height
		);
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
