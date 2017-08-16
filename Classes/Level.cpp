#include "Level.hpp"

Level::Level(Game *_game, sf::RenderWindow *_window, std::string levelpath) :
	GameState(_game, _window), framecount(0)
{	
	uiview = new View(_window, 16, 3, 0, -80, 0, 0);
	eventHandler.subscribe(uiview);
	
	buildLevel(levelpath);
}
Level::~Level() {
	std::cout << "level reset" << std::endl;
	destroyAllThings();
	delete navmap;
	delete levelview;
	delete uiview;
}

void Level::buildLevel(std::string levelFile)
{
	LuaHelper mrLua;

	if (mrLua.loadFile(levelFile))
	{
		std::cout << mrLua.to<std::string>(-1);
		std::cin.get();
		//some kind of error!
		//throw 1;
	}
	else
	{
		//level width and height
		w = mrLua.getvalue<int>("width");
		h = mrLua.getvalue<int>("height");

		std::cout << "level dimensions: " << w << " x " << h << std::endl;

		// Create map
		navmap = new NavMap(w, h);
	
		// Create levelview
		levelview = new LevelView(window, w, h, 0, 0, 0, 80);
		eventHandler.subscribe(levelview);

		//buildings
		mrLua.pushtable("buildings");

		lua_pushnil(mrLua.LuaInstance); //start at the start
		while (lua_next(mrLua.LuaInstance,1) != 0) //"buildings" table is at index 1 in the stack
		{
			int x,y; //temp. since we need these to even construct building!

			//x and y
			x = mrLua.getfield<int>("x");
			y = mrLua.getfield<int>("y");

			Building* b = createBuilding(x, y);

			//type
			switch(mrLua.getfield<int>("type")) //get the value returned from the top
			{
				case 1:
					b->type = HOME;
					break;
				case 2:
					b->type = BARBER;
					break;
				case 3:
					b->type = PIESHOP;
					break;
				default:
					b->type = DERELICT;
					break;
			}

			//pop the table, leaving the key ready for next iteration
			lua_pop(mrLua.LuaInstance, 1);
		}
	}
}

void Level::pause() { }
void Level::resume(Returny *returny) {
	if (returny->type == Returny::killer_returny)
		game->stateFinished(this, Returny(Returny::killer_returny));
}

void Level::update()
{
	updateObjects();
	destroyThings();	// Removed destroyed objects.
}

void Level::handleEvent(Event* event)
{
	// Keys
	if (event->type == KEYPRESS) {
		if (event->key == K_Q)
			game->stateFinished(this, Returny(Returny::killer_returny));
		if (event->key == K_ESC)
			game->stateFinished(this, Returny(Returny::empty_returny));
		if (event->key == K_P)
			createPlaceable();
	}
}

Unit* Level::createUnit(int atX, int atY) {
	Unit *u = new Unit(navmap, levelview, atX, atY);
	units.push_back(u);
	levelview->addResponder(u);
	std::cout << "added unit " << u << " (now " << units.size() << ")" << std::endl;
	return u;
}
Building* Level::createBuilding(int atX, int atY) {
	Building *b = new Building(atX, atY);
	buildings.push_back(b);
	levelview->addResponder(b);
	navmap->addBuilding(b);
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
			levelview->removeResponder(*i);
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
	if (framecount == 90) framecount = 0;
	if (10 == framecount++) createUnit(rand()%w, rand()%h);	// Create a new unit every so often
	
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
	drawRect(sf::Color(0, 0, 0, 50), 0, 0, grid_w, grid_h);

	// Draw buildings
	for (int i=0, n = buildings->size(); i < n; i++)
		drawMappedObj((*buildings)[i]);
	// Draw placeables
	for (int i=0, n = placeables->size(); i < n; i++)
		drawMappedObj((*placeables)[i]);
	// Draw units
	for (int i=0, n = units->size(); i < n; i++)
		drawMappedObj((*units)[i]);
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
