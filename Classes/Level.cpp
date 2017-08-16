#include "Level.hpp"
#include "Game.hpp"
#include "ResponderMap.hpp"
#include "NavMap.hpp"
#include "Building.hpp"
#include "Placeable.hpp"
#include "Unit.hpp"

Level::Level(Game *_game, W *_theW, std::string levelpath) : GameState(_game, _theW) {
	framecount = 0;

	buildLevel(levelpath);
	
	JenniferAniston aniston(theW, BOTTOM_LEFT, FIXED, PROPORTIONAL, 0, 0, 1, 0.1);
	uibarview = new UIBarView(theW, aniston, this);
	responderMap.addResponder(uibarview);
	
	// Subscribe to screenedge events
	responderMap.subscribeToEventType(this, Event::SCREENEDGE_LEFT);
	responderMap.subscribeToEventType(this, Event::SCREENEDGE_RIGHT);
	responderMap.subscribeToEventType(this, Event::SCREENEDGE_TOP);
	responderMap.subscribeToEventType(this, Event::SCREENEDGE_BOTTOM);
	
	// Key subscriptions
	responderMap.subscribeToKey(this, Event::K_Q);
	responderMap.subscribeToKey(this, Event::K_P);
}
Level::~Level() {
	std::cout << "level destruct" << std::endl;
	destroyAllThings();
	delete navmap;
	delete levelview;
	delete uibarview;
}

void Level::buildLevel(std::string fileName)
{
	std::cout << "path before: " << fileName << std::endl;
	fileName = theW->pathForResource(fileName);
	std::cout << "path after: " << fileName << std::endl;
	LuaHelper mrLua;
	
	if (mrLua.loadFile(fileName))
	{
		//some kind of error!
		std::cout << mrLua.to<std::string>(-1);
		std::cin.get();
		//throw 1;
		return;
	}
	
	// Set level width and height
	w = mrLua.getvalue<int>("width");
	h = mrLua.getvalue<int>("height");
	std::cout << "level dimensions: " << w << " x " << h << std::endl;
	
	// Create map
	navmap = new NavMap(w, h);
	levelResponderMap = new ResponderMap(w, h);
	
	// Create levelview
	JenniferAniston aniston(theW, TOP_LEFT, FIXED, PROPORTIONAL, 0, 0, 1, 1);
	levelview = new LevelView(theW, aniston, levelResponderMap, &buildings, &placeables, &units, w, h);
	responderMap.addResponder(levelview);
	
	// Lua: add buildings
	mrLua.pushtable("buildings");
	
	lua_pushnil(mrLua.LuaInstance);					// Start at the start
	while (lua_next(mrLua.LuaInstance, 1) != 0)		// "buildings" table is at index 1 in the stack
	{
		int x = mrLua.getfield<int>("x");
		int y = mrLua.getfield<int>("y");
		
		Building* b = createBuilding(x, y);
		
		switch(mrLua.getfield<int>("type")) // Get the value returned from the top
		{
			case 1:
				b->type = Building::HOME;
				break;
			case 2:
				b->type = Building::BARBER;
				break;
			case 3:
				b->type = Building::PIESHOP;
				break;
			default:
				b->type = Building::DERELICT;
				break;
		}
		
		lua_pop(mrLua.LuaInstance, 1);	// Pop the table, leaving key ready for next iteration
	}
}

void Level::pause() {
	// pausery
}
void Level::resume(Returny *returny) {
	if (returny->type == Returny::killer_returny)
		game->stateFinished(this, Returny(Returny::killer_returny));
}
void Level::update() {
	if (framecount == 60) framecount = 0;
	if (framecount++ == 10) createUnit(rand()%w, rand()%h);		// Create a new unit every so often
	
	for (int i=0; i < units.size(); i++)
		units[i]->update();					// Call update() on all TLOs
	
	destroyThings();	// Removed destroyed objects.
}
void Level::draw() {
	levelview->_draw();
	uibarview->_draw();
}
void Level::setResolution(int _w, int _h) {
	GameState::setResolution(_w, _h);
	levelview->updatePosition();
	uibarview->updatePosition();
}
	
void Level::receiveEvent(Event *ev) {
	if (ev->type == Event::SCREENEDGE_LEFT)		levelview->scroll(LEFTWARD);
	if (ev->type == Event::SCREENEDGE_RIGHT)	levelview->scroll(RIGHTWARD);
	if (ev->type == Event::SCREENEDGE_TOP)		levelview->scroll(UPWARD);
	if (ev->type == Event::SCREENEDGE_BOTTOM)	levelview->scroll(DOWNWARD);
	if (ev->type == Event::KEYPRESS) {
		if (ev->key == Event::K_Q)
			game->stateFinished(this, Returny(Returny::killer_returny));
		if (ev->key == Event::K_ESC)
			game->stateFinished(this, Returny(Returny::empty_returny));
		if (ev->key == Event::K_P)
			createPlaceable();
	}
}

void Level::handleCloseEvent() {
	GameState::handleCloseEvent();		// i.e. just quit for now
}

Unit* Level::createUnit(int atX, int atY) {
	Unit *u = new Unit(navmap, atX, atY);
	units.push_back(u);
	levelResponderMap->addMappedObj(u);
	std::cout << "added unit " << u << " (now " << units.size() << ")" << std::endl;
	return u;
}
Building* Level::createBuilding(int atX, int atY) {
	Building *b = new Building(atX, atY);
	buildings.push_back(b);
	levelResponderMap->addMappedObj(b);
	responderMap.subscribeToKey(b, Event::K_L);
	navmap->addBuilding(b);
	std::cout << "added building " << b << " (now " << buildings.size() << ")" << std::endl;
	return b;
}
void Level::createPlaceable() {
	Placeable *p = new Placeable(navmap, levelResponderMap);
	if (!levelResponderMap->requestPrivilegedEventResponderStatus(p)) {
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
			levelResponderMap->removeMappedObj(*i);
			navmap->removeImpassableObject(*i);
			delete *i;
			i = placeables.erase(i);
		}
		else i++;
	for (std::vector<Building*>::iterator i = buildings.begin(); i < buildings.end(); )
		if ((*i)->destroyed) {
			levelResponderMap->removeMappedObj(*i);
			responderMap.unsubscribeFromAllKeys(*i);
			navmap->removeImpassableObject(*i);
			delete *i;
			i = buildings.erase(i);
		}
		else i++;
	for (std::vector<Unit*>::iterator i = units.begin(); i < units.end(); )
		if ((*i)->destroyed) {
			levelResponderMap->removeMappedObj(*i);
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


#include "Button.hpp"

LevelView::LevelView(
	W *_theW, JenniferAniston &_aniston,
	ResponderMap *_levelRM, std::vector<Building*> *_buildings, std::vector<Placeable*> *_placeables, std::vector<Unit*> *_units,
	int _level_width, int _level_height
) :
	View(_theW, _aniston),
	levelResponderMap(_levelRM), buildings(_buildings), placeables(_placeables), units(_units),
	level_width(_level_width), level_height(_level_height),
	scroll_x(0), scroll_y(0)
{
	gridsize = 20;		// Pixel size of a level block
}

void LevelView::drawMappedObj(MappedObj *obj) {
	// Calculate position, accounting for scroll
	int atX = (obj->x + obj->a) * gridsize - scroll_x;
	int atY = (obj->y + obj->b) * gridsize - scroll_y;
	
	for (int i=0; i < obj->ground_plan.size(); i++) {
		intcoord c = obj->ground_plan[i];
		theW->drawRect(atX + c.x*gridsize, atY + c.y*gridsize, gridsize, gridsize, obj->col());
	}
}

void LevelView::draw() {
	// Draw background
	theW->drawRect(0, 0, width, height, BLACK);

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

void LevelView::processMouseEvent(Event *ev) {
	// Convert event from View coordinates to grid coordinates
	ev->x += scroll_x;
	ev->y += scroll_y;
	ev->a = (float) (ev->x%gridsize) / (float) gridsize;
	ev->b = (float) (ev->y%gridsize) / (float) gridsize;
	ev->x = ev->x / gridsize;
	ev->y = ev->y / gridsize;
	
	levelResponderMap->dispatchEvent(ev);
}

void LevelView::scroll(direction dir) {
	int scrolldist = 10;
	
	if (dir == UPWARD)			scroll_y -= scrolldist;
	else if (dir == DOWNWARD)	scroll_y += scrolldist;
	else if (dir == LEFTWARD)	scroll_x -= scrolldist;
	else if (dir == RIGHTWARD)	scroll_x += scrolldist;
	
	if (scroll_x < 0) scroll_x = 0;
	if (scroll_y < 0) scroll_y = 0;
	
	int max_scroll_x = level_width * gridsize - width;
	int max_scroll_y = level_height * gridsize - height;
	if (scroll_x >= max_scroll_x) scroll_x = max_scroll_x;
	if (scroll_y >= max_scroll_y) scroll_y = max_scroll_y;
}


UIBarView::UIBarView(W *_theW, JenniferAniston &aniston, Level *_level) :
	View(_theW, aniston), buttonMap(width, height), level(_level)
{
	createplaceable_btn = new Button(this, 10, 10, 20, 20);
	buttonMap.addResponder(createplaceable_btn);
	buttons.push_back(createplaceable_btn);
}

UIBarView::~UIBarView()
{
	
}

void UIBarView::buttonClick(Button *btn) {
	if (btn == createplaceable_btn)
		level->createPlaceable();
}

void UIBarView::processMouseEvent(Event *ev) {
	buttonMap.dispatchEvent(ev);
}

void UIBarView::draw() {
	theW->drawRect(0, 0, width, height, BLACK);
	for (int i=0; i < buttons.size(); i++) {
		Button *b = buttons[i];
		theW->drawRect(b->x, b->y, b->width, b->height, b->col());
	}
}

void UIBarView::updatePosition() {
	View::updatePosition();
	buttonMap.setSize(width, height);
}
