#include "Level.hpp"
#include "Game.hpp"
#include "ResponderMap.hpp"
#include "NavMap.hpp"
#include "Building.hpp"
#include "Placeable.hpp"
#include "Unit.hpp"
#include "LuaHelper.hpp"

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
	responderMap.subscribeToKey(this, Event::K_ESC);
	responderMap.subscribeToKey(this, Event::K_Q);
	responderMap.subscribeToKey(this, Event::K_C);
	responderMap.subscribeToKey(this, Event::K_S);
}
Level::~Level() {
	std::cout << "level destruct" << std::endl;
	destroyAllThings();
	delete navmap;
	delete levelview;
	delete uibarview;
}

void Level::buildLevel(std::string levelname) {
	theW->log("Level::buildLevel() called...");

	LuaHelper mrLua(theW);
	
	std::string path = theW->resourcesPath;
	path.append(levelname);
	
	if (!mrLua.loadFile(path.c_str()))
		throw MsgException("Could not read level file.");	// To get error from Mr Lua: mrLua.to<std::string>(-1).c_str()
	lua_State *L = mrLua.LuaInstance;

	// Initialize TLO classes
	if (!Building::initialize(theW))  throw MsgException("Couldn't read building info.");
	if (!Unit::initialize(theW))      throw MsgException("Couldn't read unit info.");
	if (!Placeable::initialize(theW)) throw MsgException("Couldn't read placeable info.");
	
	// Set level width and height
	try {
		w = mrLua.getvalue<int>("width");
		h = mrLua.getvalue<int>("height");
	} catch (MsgException &exc) {
		std::string s = "Couldn't get level's dimensions: ";
		s.append(exc.msg);
		throw MsgException(s.c_str());
	}
	
	// Create map
	navmap = new NavMap(w, h);
	levelResponderMap = new ResponderMap(w, h);
	
	// Create levelview
	JenniferAniston aniston(theW, TOP_LEFT, PFIXED, PPROPORTIONAL, 0, 0, 1, 1);
	levelview = new LevelView(theW, aniston, levelResponderMap, &buildings, &placeables, &units, &staff, w, h);
	responderMap.addResponder(levelview);
	
	// Get allowed buildings
	try {
		mrLua.pushtable("allowedBuildings");
		lua_pushnil(L);									// S: -1 nil; -2 table
		int n = 0;
		std::string s = "allowedBuildings: ";
		while (lua_next(L, -2) != 0) {					// S: -1 val; -2 key; -3 table
			if (!lua_isstring(L, -1)) continue;
			allowedBuildings.push_back(lua_tostring(L, -1));
			s.append(lua_tostring(L, -1)); s.append(", ");
			lua_pop(L, 1);								// S: -1 key; -2 table
			n++;
		}
		if (n > 0) {
			s.erase(s.size() - 2);
			W::log(s.c_str());
		}
		else
			W::log("No building types in allowedBuildings.");
		lua_settop(L, 0);	// S: ~
	} catch (MsgException &exc) {
		std::string s = "Error getting list of allowed buildings for level: ";
		s.append(exc.msg);
		throw MsgException(s.c_str());
	}
	
	// Populate level with buildings
	try {
		mrLua.pushtable("buildings");		// S: buildings
		lua_pushnil(L);						// S: -1 nil; -2 buildings
		while (lua_next(L, -2) != 0) {		// S: -1 building; -2 key; -3 buildings
			if (lua_type(L, -1) != LUA_TTABLE) {
				lua_pop(L, 1);
				continue;
			}
			// Set position & type
			int bX = mrLua.getfield<int>("x");
			int bY = mrLua.getfield<int>("y");
			const char *bType = mrLua.getfield<const char *>("type");
			
			// Set ground plan
			std::vector<intcoord> gp;
			if (!mrLua.pushSubtable("groundplan")) throw MsgException("Could not find groundplan table.");
			lua_pushnil(L);									// S: -1 nil; -2 groundplan; -4 building; -5 key; -6 buildings
			for (int i=0; lua_next(L, -2); i++) {			// S: -1 coords; -2 key; -3 groundplan; ...
				if (!lua_istable(L, -1)) {
					lua_pop(L, 1);
					continue;
				}
				intcoord c = { mrLua.getfield<int>(1), mrLua.getfield<int>(2) };
				gp.push_back(c);
				lua_pop(L, 1);								// S: -1 key; -2 groundplan; -3 building; -4 key; -5 buildings
			}												// S: -1 groundplan; -2 building; -3 key; -4 buildings
			lua_pop(L, 1);									// S: -1 building; -2 key; -3 building
			
			// Set doors (optional... !)
			std::vector<door> doors;
			if (mrLua.pushSubtable("doors")) {				// S: -1 doors; -2 building; -3 key; -4 buildings
				door d;
				lua_pushnil(L);								// S: -1 nil; -2 doors; ...
				while (lua_next(L, -2)) {					// S: -1 door; -2 key; -3 doors; ...
					if (!lua_istable(L, -1)) {
						lua_pop(L, 1);
						continue;
					}
					d.coord.x = mrLua.getfield<int>(1);
					d.coord.y = mrLua.getfield<int>(2);
					const char *dir = mrLua.getfield<const char *>(3);
					if (strcmp(dir, "upward") == 0)         d.orientation = UPWARD;
					else if (strcmp(dir, "downward") == 0)  d.orientation = DOWNWARD;
					else if (strcmp(dir, "leftward") == 0)  d.orientation = LEFTWARD;
					else if (strcmp(dir, "rightward") == 0) d.orientation = RIGHTWARD;
					doors.push_back(d);
					lua_pop(L, 1);							// S: -1 key; -2 doors; -3 building; -4 key; -5 buildings
				}											// S: -1 doors; -2 building; -3 key; -4 buildings
			}
			lua_pop(L, 2);									// S: -1 key; buildings
			
			createBuilding(bX, bY, bType, &gp, &doors);
		}
		lua_settop(L, 0);	// S: ~
	} catch (MsgException &exc) {
		std::string s = "Couldn't load buildings: ";
		s.append(exc.msg);
		throw MsgException(s.c_str());
	}
	
	// Add spawn points
	try {
		mrLua.pushtable("spawnPoints");
		lua_pushnil(L);						// S: -1 nil; -2 table
		while (lua_next(L,1) != 0) {		// S: -1 val; -2 key; -3 table
			spawnPoints.push_back(new SpawnPoint(
				mrLua.getfield<int>("x"),
				mrLua.getfield<int>("y"),
				mrLua.getfield<std::string>("name"),
				mrLua.getfield<int>("rate")
			));
			lua_pop(L, 1);					// S: -1 key; -2 table
		}
	} catch (MsgException &exc) {
		std::string s = "Couldn't add spawn points: ";
		s.append(exc.msg);
		throw MsgException(s.c_str());
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
	intcoord c;
	for (int i=0, n = spawnPoints.size(); i < n; i++)
		if (spawnPoints[i]->spawn(&c))
			createUnit(c.x, c.y, "civilian");
	
	// Update TLOs
	for (int i=0, n = units.size(); i < n; i++) units[i]->update();
	for (int i=0, n = staff.size(); i < n; i++) staff[i]->update();
	
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
		if (ev->key == Event::K_Q) game->stateFinished(this, Returny(Returny::killer_returny));
		if (ev->key == Event::K_ESC) game->stateFinished(this, Returny(Returny::empty_returny));
		if (ev->key == Event::K_C) createPlaceable("barberschair");
		if (ev->key == Event::K_S) createPlaceable("sofa");
	}
}

void Level::handleCloseEvent() {
	GameState::handleCloseEvent();		// i.e. just quit for now
}

Unit* Level::createUnit(int atX, int atY, const char *type) {
	std::vector<Unit*> *vctr = &units;
	if (strcmp(type, "staff") == 0) {
		// Find the asylum (spawn building) amongst our buildings
		// Note: this method will use the first asylum it finds
		for (std::vector<Building*>::iterator i = buildings.begin(); i < buildings.end(); i++)
			if ((*i)->type == "asylum") {
				atX = (*i)->x;
				atY = (*i)->y;
				break;
			}
		vctr = &staff;
	}
	Unit *u = new Unit(navmap, atX, atY, type);
	vctr->push_back(u);
	levelResponderMap->addMappedObj(u);
	std::cout << "added unit " << u << " of type '" << type << "' (now " << vctr->size() << ")" << std::endl;
	return u;
}
Building* Level::createBuilding(int atX, int atY, const char *type, std::vector<intcoord> *groundplan, std::vector<door> *doors) {
	Building *b = new Building(atX, atY, type, groundplan, doors);
	buildings.push_back(b);
	levelResponderMap->addMappedObj(b);
	responderMap.subscribeToKey(b, Event::K_L);
	navmap->addBuilding(b);
	std::cout << "added building " << b << " (now " << buildings.size() << ")" << std::endl;
	return b;
}
void Level::createPlaceable(const char *type) {
	Placeable *p = new Placeable(navmap, levelResponderMap, type);
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
	for (std::vector<Unit*>::iterator i = staff.begin(); i < staff.end(); )
		if ((*i)->destroyed) {
			levelResponderMap->removeMappedObj(*i);
			delete *i;
			i = staff.erase(i);
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
	ResponderMap *_levelRM,
	std::vector<Building*> *_buildings, std::vector<Placeable*> *_placeables, std::vector<Unit*> *_units, std::vector<Unit*> *_staff,
	int _level_width, int _level_height
) :
	View(_theW, _aniston),
	levelResponderMap(_levelRM), buildings(_buildings), placeables(_placeables), units(_units), staff(_staff), 
	level_width(_level_width), level_height(_level_height),
	scroll_x(0), scroll_y(0)
{
	gridsize = 20;		// Pixel size of a level block
}

void LevelView::drawMappedObj(MappedObj *obj) {
	// Calculate position, accounting for scroll
	int atX = (obj->x + obj->a) * gridsize - scroll_x;
	int atY = (obj->y + obj->b) * gridsize - scroll_y;
	
	if (atX >= width + gridsize || atY >= height + gridsize) return;
	
	const char *col = obj->col();
	for (int i=0; i < obj->groundplan.size(); i++) {
		intcoord c = obj->groundplan[i];
		theW->drawRect(atX + c.x*gridsize, atY + c.y*gridsize, gridsize, gridsize, col);
	}
}

void LevelView::draw() {
	theW->drawRect(0, 0, width, height, "black");
	for (int i=0, n = buildings->size(); i < n; i++)  drawMappedObj((*buildings)[i]);
	for (int i=0, n = placeables->size(); i < n; i++) drawMappedObj((*placeables)[i]);
	for (int i=0, n = units->size(); i < n; i++)      drawMappedObj((*units)[i]);
	for (int i=0, n = staff->size(); i < n; i++)      drawMappedObj((*staff)[i]);
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
	
	if (dir == UPWARD)         scroll_y -= scrolldist;
	else if (dir == DOWNWARD)  scroll_y += scrolldist;
	else if (dir == LEFTWARD)  scroll_x -= scrolldist;
	else if (dir == RIGHTWARD) scroll_x += scrolldist;
	
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

	createstaff_btn = new Button(this, 40, 10, 20, 20);
	buttonMap.addResponder(createstaff_btn);
	buttons.push_back(createstaff_btn);
}

UIBarView::~UIBarView()
{
	
}

void UIBarView::buttonClick(Button *btn) {
	if (btn == createplaceable_btn) level->createPlaceable("barberschair");
	if (btn == createstaff_btn)     level->createUnit(0, 0, "staff");
}

void UIBarView::processMouseEvent(Event *ev) {
	buttonMap.dispatchEvent(ev);
}

void UIBarView::draw() {
	theW->drawRect(0, 0, width, height, "black");
	for (int i=0; i < buttons.size(); i++) {
		Button *b = buttons[i];
		theW->drawRect(b->x, b->y, b->width, b->height, b->col());
	}
}

void UIBarView::updatePosition() {
	View::updatePosition();
	buttonMap.setSize(width, height);
}
