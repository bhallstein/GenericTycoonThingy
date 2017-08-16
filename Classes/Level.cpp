#include "Level.hpp"

Level::Level(Game *_game, sf::RenderWindow *_window, std::string levelpath) :
	GameState(_game, _window), framecount(0)
{	
	uiview = new View(_window, 16, 3, 0, -80, 0, 0);
	uiview->createEventResponderMap();
	eventHandler.subscribe(uiview);
	
	buildLevel(readLevel(levelpath));
}

Level::~Level() {
	
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
	std::cout << "level dimensions: " << w << " x " << h << std::endl;
	
	// Create map
	navmap = new NavMap(w, h);
	
	// Create levelview
	levelview = new LevelView(window, w, h, 0, 0, 0, 80);
	levelview->createEventResponderMap();					// Make levelview respond to mouse events
	eventHandler.subscribe(levelview);						// 

	BOOST_FOREACH(ptree::value_type &obj, levelFile.get_child("level"))
	{
		if(obj.first == "building")
			createBuilding(obj.second.get<int>("x",0), obj.second.get<int>("y",0));
	}
}


void Level::reset()
{
	std::cout << "level reset" << std::endl;
	destroyAllThings();
	delete navmap;
	delete levelview;
	delete uiview;
}

//To be done later ;)
void Level::pause() { }
void Level::resume() { }

void Level::update()
{
	updateObjects();
	destroyThings();	// Removed destroyed objects.
}

void Level::handleEvent(Event* event)
{
	// Keys
	if (event->type == KEYPRESS) { 
		if (event->key == K_P)
			createPlaceable();
	}
}

Unit* Level::createUnit(int atX, int atY) {
	Unit *u = new Unit(navmap, atX, atY);
	units.push_back(u);
	std::cout << "added unit " << u << " (now " << units.size() << ")" << std::endl;
	return u;
}
Building* Level::createBuilding(int atX, int atY) {
	Building *b = new Building(navmap, atX, atY);
	buildings.push_back(b);
	levelview->addResponder(b);
	eventHandler.subscribe(b, K_L);
	std::cout << "added building " << b << " (now " << buildings.size() << ")" << std::endl;
	return b;
}
void Level::createPlaceable() {
	Placeable *p = new Placeable(navmap, levelview);
	if (!levelview->requestPrivilegedEventResponderStatus(p)) {
		delete p;
		return;
	}
	placeables.push_back(p);
	std::cout << "added placeable " << p << " (now " << placeables.size() << ")" << std::endl;
	return;
}
void Level::destroyThings() {
	for (std::vector<Placeable*>::iterator i = placeables.begin(); i < placeables.end(); )
		if ((*i)->destroyed) {
			levelview->removeResponder(*i);
			navmap->removeImpassableObject(*i);
			delete *i;
			i = placeables.erase(i);
		}
		else i++;
	for (std::vector<Building*>::iterator i = buildings.begin(); i < buildings.end(); )
		if ((*i)->destroyed) {
			levelview->removeResponder(*i);
			navmap->removeImpassableObject(*i);
			delete *i;
			i = buildings.erase(i);
		}
		else i++;
	for (std::vector<Unit*>::iterator i = units.begin(); i < units.end(); )
		if ((*i)->destroyed) {
			delete *i;
			i = units.erase(i);
		}
		else i++;
}
void Level::destroyAllThings() {
	for (int i=0; i < placeables.size(); i++)   placeables[i]->destroyed = true;
	for (int i=0; i < buildings.size(); i++) 	buildings[i]->destroyed = true;
	for (int i=0; i < units.size(); i++) 	    units[i]->destroyed = true;
	destroyThings();
}

void Level::updateObjects() {
	if (framecount == 180) framecount = 0;
	if (10 == framecount++) createUnit(0, 0);	// Create a new unit every so often
	
	for (int i=0; i < units.size(); i++)
		units[i]->update();
}
void Level::draw()
{
	levelview->draw(&buildings, &placeables, &units);
	uiview->draw();
}


LevelView::LevelView(sf::RenderWindow *_window, int _blocks_w, int _blocks_h, int _l_offset = 0, int _t_offset = 0, int _r_offset = 0, int _b_offset = 0) :
	ScrollingView(_window, _blocks_w, _blocks_h, _l_offset, _t_offset, _r_offset, _b_offset)
{
	// oh hai
}

void LevelView::draw(std::vector<Building*> *buildings, std::vector<Placeable*> *placeables, std::vector<Unit*> *units) {

	// This is obviously a horrendous way to get info into the view, but the separation of View is nonetheless a structural
	// improvement. The sane way to do it might be to pass the game map.
	
	// Draw background
	drawRect(sf::Color(0, 0, 0, 50), 0, 0, blocks_w, blocks_h);

	// Draw buildings
	for (std::vector<Building*>::iterator i = buildings->begin(); i < buildings->end(); i++)
		drawRect(
			(*i)->col() == 'w' ? sf::Color::White : (*i)->col() == 'l' ? sf::Color::Blue : sf::Color::Black,
			(*i)->x, (*i)->y, (*i)->w, (*i)->h
		);
	// Draw placeables
	for (std::vector<Placeable*>::iterator i = placeables->begin(); i < placeables->end(); i++)
		drawRect(
			(*i)->col() == 'w' ? sf::Color::White : (*i)->col() == 'r' ? sf::Color::Red : sf::Color::Yellow,
			(*i)->x, (*i)->y, (*i)->w, (*i)->h
		);
	//Draw units
	for (std::vector<Unit*>::iterator i = units->begin(); i < units->end(); i++) {
		drawRect(
			(*i)->col() == 'r' ? sf::Color::Red : (*i)->col() == 'b' ? sf::Color::Black : sf::Color::White,
			(*i)->x, (*i)->y, (*i)->w, (*i)->h,
			(*i)->a, (*i)->b
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
