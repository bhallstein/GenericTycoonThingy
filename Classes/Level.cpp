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
	
	JenniferAniston aniston(theW, BOTTOM_LEFT, PFIXED, PPROPORTIONAL, 0, 0, 1, 0.1);
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

void Level::buildLevel(std::string levelname)
{
	LuaHelper mrLua;
	
	std::string path = theW->resourcesPath;
	path.append(levelname);

	if (!mrLua.loadFile(path)) {
		theW->warning("Error opening level file.");
		theW->log(mrLua.stackdump(mrLua.LuaInstance).c_str());
		throw MsgException("Could not read level file");		// Might actually be neater to return a bool, & throw in the constructor.
	}

	// Set level width and height
	w = mrLua.getvalue<int>("width");
	h = mrLua.getvalue<int>("height");
	//char s[100];
	//sprintf(s, "Level dimensions: %d x %d", w, h);
	
	// Create map
	navmap = new NavMap(w, h);
	levelResponderMap = new ResponderMap(w, h);
	
	// Create levelview
	JenniferAniston aniston(theW, TOP_LEFT, PFIXED, PPROPORTIONAL, 0, 0, 1, 1);
	levelview = new LevelView(theW, aniston, levelResponderMap, &buildings, &placeables, &units, w, h);
	responderMap.addResponder(levelview);
	
	//building templates
	mrLua.pushtable("availableBuildings");
	
	lua_pushnil(mrLua.LuaInstance);
	while(lua_next(mrLua.LuaInstance,1) != 0)
	{
		Building* ab = new Building();
		
		ab->type = mrLua.getfield<std::string>("type");
		ab->defaultCol.red   = mrLua.getSubfield<int>("colour","r");
		ab->defaultCol.green = mrLua.getSubfield<int>("colour","g");
		ab->defaultCol.blue  = mrLua.getSubfield<int>("colour","b");
		ab->defaultCol.alpha = mrLua.getSubfield<int>("colour","a");

		//add other template properties here

		//add this template
		availableBuildings[ab->type] = ab;
		
		//pop the table, leaving the key ready for next iteration
		lua_pop(mrLua.LuaInstance, 1);
	}
	
	//empty the stack for safety?
	lua_settop(mrLua.LuaInstance,0);
	
	//buildings
	mrLua.pushtable("buildings");
	
	lua_pushnil(mrLua.LuaInstance); //start at the start
	while(lua_next(mrLua.LuaInstance,1) != 0) //"buildings" table is at index 1 in the stack
	{
		int x,y; //temp. since we need these to even construct building!
		
		//x and y
		x = mrLua.getfield<int>("x");
		y = mrLua.getfield<int>("y");
		
		Building* b = createBuilding(x, y);
		
		//type
		Building* ab; //don't like this here as we only use it for string'd types below
		lua_getfield(mrLua.LuaInstance,-1,"type"); //push the type table to the stack
		switch(lua_type(mrLua.LuaInstance,-1))
		{
			case LUA_TSTRING:
				//look up properties from the named template class
				ab = availableBuildings[lua_tostring(mrLua.LuaInstance,-1)];
				
				//set properties from the template
				b->type = ab->type;
				b->defaultCol = ab->defaultCol;
				
				//we should rewrite this so b = ab; then override properties with building specific stuff?
				break;
			case LUA_TTABLE:
				//set properties as returned from lua
				b->type = mrLua.getfield<std::string>("type");
				b->defaultCol.red   = mrLua.getSubfield<int>("colour","r");
				b->defaultCol.green = mrLua.getSubfield<int>("colour","g");
				b->defaultCol.blue  = mrLua.getSubfield<int>("colour","b");
				b->defaultCol.alpha = mrLua.getSubfield<int>("colour","a");
				break;
		}
		//pop the type table
		lua_pop(mrLua.LuaInstance, 1);
		
		//any other properties that aren't templated here
		
		//pop the table, leaving the key ready for next iteration
		lua_pop(mrLua.LuaInstance, 1);
	}
	
	//empty the stack
	lua_settop(mrLua.LuaInstance,0);
	
	//Spawn Points!
	mrLua.pushtable("spawnPoints");
	lua_pushnil(mrLua.LuaInstance); //start at the start
	while (lua_next(mrLua.LuaInstance,1) != 0) //spawnpointicles
	{
		spawnPoints.push_back(new SpawnPoint(&mrLua));
		
		//pop the table, leaving the key ready for next iteration
		lua_pop(mrLua.LuaInstance, 1);
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
	
	for(int i=0; i < spawnPoints.size(); i++)
	{
		std::vector<int> spawnLoc = spawnPoints[i]->Spawn();
		if(spawnLoc.size() > 1) //does it have both co-ords?
			createUnit(spawnLoc[0], spawnLoc[1]);
	}
	
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
	if (ev->type == Event::SCREENEDGE_LEFT)			levelview->scroll(LEFTWARD);
	else if (ev->type == Event::SCREENEDGE_RIGHT)	levelview->scroll(RIGHTWARD);
	else if (ev->type == Event::SCREENEDGE_TOP)		levelview->scroll(UPWARD);
	else if (ev->type == Event::SCREENEDGE_BOTTOM)	levelview->scroll(DOWNWARD);
	else if (ev->type == Event::KEYPRESS) {
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
	theW->drawRect(0, 0, width, height, colour(_BLACK_));
	for (int i=0, n = buildings->size(); i < n; i++)	drawMappedObj((*buildings)[i]);
	for (int i=0, n = placeables->size(); i < n; i++)	drawMappedObj((*placeables)[i]);
	for (int i=0, n = units->size(); i < n; i++)		drawMappedObj((*units)[i]);
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
	theW->drawRect(0, 0, width, height, colour(_BLACK_));
	for (int i=0; i < buttons.size(); i++) {
		Button *b = buttons[i];
		theW->drawRect(b->x, b->y, b->width, b->height, b->col());
	}
}

void UIBarView::updatePosition() {
	View::updatePosition();
	buttonMap.setSize(width, height);
}
