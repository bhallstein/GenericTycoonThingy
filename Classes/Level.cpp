#include "Level.hpp"
#include "Game.hpp"
#include "ResponderMap.hpp"
#include "NavMap.hpp"
#include "Building.hpp"
#include "Furniture.hpp"
#include "Placeable.hpp"
#include "Unit.hpp"
#include "LuaHelper.hpp"
#include "Callback.hpp"

Level::Level(Game *_game, W *_theW, std::string levelpath) : GameState(_game, _theW)
{
	framecount = 0;
	
	currentlyEditedBuilding = NULL;
	levelview = NULL;
	uibarview = NULL;
	furniturePurchasingView = NULL;
	hiringUIView = NULL;

	buildLevel(levelpath);
	
	JenniferAniston aniston(theW, BOTTOM_LEFT, PFIXED, PPROPORTIONAL, 0, 0, 1, 0.1);
	uibarview = new UIBarView(theW, aniston);
	responderMap.addResponder(uibarview);
	uibarview->subscribe("create barberschair", new Callback(&Level::createBarbersChair, this));
	uibarview->subscribe("open hiring ui view", new Callback(&Level::openHiringView, this));
	
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
Level::~Level()
{
	std::cout << "level destruct" << std::endl;
	destroyAllThings();
	delete navmap;
	delete levelview;
	delete uibarview;
	delete furniturePurchasingView;
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
	if (!Furniture::initialize(theW)) throw MsgException("Couldn't read furniture info.");
	
	// Set level width and height
	try {
		w = mrLua.getvalue<int>("width");
		h = mrLua.getvalue<int>("height");
	} catch (MsgException &exc) {
		std::string s = "Couldn't get level's dimensions: ";
		s.append(exc.msg);
		throw MsgException(s.c_str());
	}

	//set level (player's) money to the starting balance
	try {
		money = mrLua.getvalue<int>("startingBalance");
	} catch (MsgException &exc) {
		std::string s = "Couldn't get level's starting balance: ";
		s.append(exc.msg);
		throw MsgException(s.c_str());
	}
	
	// Create map
	navmap = new NavMap(w, h);
	levelResponderMap = new ResponderMap(w, h);
	
	// Create levelview
	JenniferAniston aniston(theW, TOP_LEFT, PFIXED, PPROPORTIONAL, 0, 0, 1, 1);
	levelview = new LevelView(theW, aniston, levelResponderMap, &buildings, &furniture, &units, &staff, w, h);
	responderMap.addResponder(levelview);
	
	// Get allowed buildings
	try {
		mrLua.pushtable("allowedBuildings");
		lua_pushnil(L);									// S: -1 nil; -2 table
		int n = 0;
		std::string s = "allowedBuildings: ";
		while (lua_next(L, -2) != 0) {					// S: -1 val; -2 key; -3 table
			if (!lua_isstring(L, -1)) {
				lua_pop(L, 1);
				continue;
			}
			allowedBuildings.push_back(lua_tostring(L, -1));
			s += lua_tostring(L, -1); s += ", ";
			n++;
			lua_pop(L, 1);								// S: -1 key; -2 table
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
			
			createBuilding(bX, bY, bType, &gp, &doors);		// TODO: check for NULL here?
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
	if (furniturePurchasingView != NULL) furniturePurchasingView->_draw();
	if (hiringUIView != NULL) hiringUIView->_draw();
}
void Level::setResolution(int _w, int _h) {
	GameState::setResolution(_w, _h);
	levelview->updatePosition();
	uibarview->updatePosition();
}

void Level::receiveEvent(Event *ev) {
	if (ev->type == Event::SCREENEDGE_LEFT)        levelview->scroll(LEFTWARD);
	else if (ev->type == Event::SCREENEDGE_RIGHT)  levelview->scroll(RIGHTWARD);
	else if (ev->type == Event::SCREENEDGE_TOP)    levelview->scroll(UPWARD);
	else if (ev->type == Event::SCREENEDGE_BOTTOM) levelview->scroll(DOWNWARD);
	else if (ev->type == Event::KEYPRESS) {
		if (ev->key == Event::K_Q)   game->stateFinished(this, Returny(Returny::killer_returny));
		if (ev->key == Event::K_ESC) game->stateFinished(this, Returny(Returny::empty_returny));
		if (ev->key == Event::K_C)   createFurniture("barberschair");
		if (ev->key == Event::K_S)   createFurniture("sofa");
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
	Unit *u = new Unit(levelResponderMap, navmap, type);
	if (u->init(atX, atY)) {
		vctr->push_back(u);
		std::cout << "added unit " << u << " of type '" << type << "' (now " << vctr->size() << ")" << std::endl;
		return u;
	}
	return NULL;
}
Building* Level::createBuilding(int atX, int atY, const char *type, std::vector<intcoord> *groundplan, std::vector<door> *doors) {
	Building *b = new Building(levelResponderMap, navmap, type, groundplan, doors, this);
	if (b->init(atX, atY)) {
		buildings.push_back(b);
		std::cout << "added building " << b << " (now " << buildings.size() << ")" << std::endl;
		return b;
	}
	return NULL;
}
void Level::createFurniture(const char *type) {
	Furniture *f = new Furniture(levelResponderMap, navmap, type, currentlyEditedBuilding);
	if (f->init(-100, -100)) {
		furniture.push_back(f);
		std::cout << "added furniture " << f << " (now " << furniture.size() << ")" << std::endl;
	}
}
void Level::createBarbersChair() { createFurniture("barberschair"); }
void Level::createSofa() { createFurniture("sofa"); }
void Level::createStaffUnit() { 
	if(chargePlayer(Unit::getUnitHireCost("staff")))
		createUnit(0, 0, "staff");
	else
		W::log("Not enough money to hire a Staff!");
}

void Level::destroyThings() {
	for (std::vector<Furniture*>::iterator i = furniture.begin(); i < furniture.end(); )
		if ((*i)->destroyed) {
			delete *i;
			i = furniture.erase(i);
		}
		else i++;
	for (std::vector<Building*>::iterator i = buildings.begin(); i < buildings.end(); )
		if ((*i)->destroyed) {
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
	for (std::vector<Unit*>::iterator i = staff.begin(); i < staff.end(); )
		if ((*i)->destroyed) {
			delete *i;
			i = staff.erase(i);
		}
		else i++;
}
void Level::destroyAllThings() {
	for (int i = 0; i < furniture.size(); i++) furniture[i]->destroyed = true;
	for (int i = 0; i < buildings.size(); i++) buildings[i]->destroyed = true;
	for (int i = 0; i < units.size(); i++)     units[i]->destroyed = true;
	destroyThings();
}

void Level::openFurniturePurchasingView(Building *b) {
	if (furniturePurchasingView != NULL) closeFurniturePurchasingView();
	currentlyEditedBuilding = b;
	JenniferAniston aniston(theW, TOP_LEFT, PFIXED, PFIXED, 47, 47, 140, 220);
	furniturePurchasingView = new FurniturePurchasingUIView(theW, aniston, b->b_allowedFurniture);
	responderMap.addResponder(furniturePurchasingView);
	
	furniturePurchasingView->subscribe("close", new Callback(&Level::closeFurniturePurchasingView, this));
	furniturePurchasingView->subscribe("create barberschair", new Callback(&Level::createBarbersChair, this));
	furniturePurchasingView->subscribe("create sofa", new Callback(&Level::createSofa, this));
}
void Level::closeFurniturePurchasingView() {
	delete furniturePurchasingView;
	furniturePurchasingView = NULL;
	currentlyEditedBuilding = NULL;
}
void Level::openHiringView() {
	if (hiringUIView != NULL) closeHiringView();
	JenniferAniston aniston(theW, BOTTOM_LEFT, PFIXED, PFIXED, 10, 90, 140, 200);
	hiringUIView = new HiringUIView(theW, aniston);
	responderMap.addResponder(hiringUIView);
	
	// Subscriptions
	hiringUIView->subscribe("close", new Callback(&Level::closeHiringView, this));
	hiringUIView->subscribe("hire staff", new Callback(&Level::createStaffUnit, this));
}
void Level::closeHiringView() {
	delete hiringUIView;
	hiringUIView = NULL;
}

//Money stuff
void Level::payPlayer(int _amount) {
	money += _amount;
}
bool Level::chargePlayer(int _amount) {
	if(money < _amount) return false; //not enough balance to pay

	decreaseMoney(_amount);
	return true;
}
void Level::decreaseMoney(int _amount) {
	money -= _amount;
}

#include "Button.hpp"

LevelView::LevelView(
	W *_theW, JenniferAniston &_aniston,
	ResponderMap *_levelRM,
	std::vector<Building*> *_buildings, std::vector<Furniture*> *_furniture, std::vector<Unit*> *_units, std::vector<Unit*> *_staff,
	int _level_width, int _level_height
) :
	View(_theW, _aniston),
	levelResponderMap(_levelRM), buildings(_buildings), furniture(_furniture), units(_units), staff(_staff), 
	level_width(_level_width), level_height(_level_height),
	scroll_x(0), scroll_y(0)
{
	gridsize = 20;		// Pixel size of a level block
}

void LevelView::drawMappedObj(MappedObj *mo) {
	// Calculate position, accounting for scroll
	int atX, atY;
	if (mo->placeableMode) {
		atX = mo->placeable->x * gridsize;
		atY = mo->placeable->y * gridsize;
	}
	else {
		atX = (mo->x + mo->a) * gridsize;
		atY = (mo->y + mo->b) * gridsize;
	}
	atX -= scroll_x;
	atY -= scroll_y;
	if (atX >= width + gridsize || atY >= height + gridsize) return;
	
	const char *col = mo->col();
	for (int i=0; i < mo->groundplan.size(); i++) {
		intcoord c = mo->groundplan[i];
		theW->drawRect(atX + c.x*gridsize, atY + c.y*gridsize, gridsize, gridsize, col);
	}
}

void LevelView::draw() {
	theW->drawRect(0, 0, width, height, "black");
	for (int i=0, n = buildings->size(); i < n; i++) drawMappedObj((*buildings)[i]);
	for (int i=0, n = furniture->size(); i < n; i++) drawMappedObj((*furniture)[i]);
	for (int i=0, n = units->size(); i < n; i++)     drawMappedObj((*units)[i]);
	for (int i=0, n = staff->size(); i < n; i++)     drawMappedObj((*staff)[i]);
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


UIBarView::UIBarView(W *_theW, JenniferAniston &_aniston) : UIView(_theW, _aniston)
{
	buttons.push_back(new Button(10, 10, 20, 20, "open hiring ui view"));
}

void UIBarView::draw() {
	theW->drawRect(0, 0, width, height, "black");
	for (int i=0, n = buttons.size(); i < n; i++) {
		Button *b = buttons[i];
		theW->drawRect(b->x, b->y, b->width, b->height, b->col());
	}
}


FurniturePurchasingUIView::FurniturePurchasingUIView(W *_theW, JenniferAniston &_aniston, std::vector<std::string> *_furnitureTypes) :
	UIView(_theW, _aniston), furnitureTypes(_furnitureTypes)
{
	buttons.push_back(new Button(7, 7, 12, 12, "close"));
	// Add buttons for creating furniture
	for (int i=0, n = furnitureTypes->size(); i < n; i++) {
		std::string s("create ");
		s += furnitureTypes->at(i);
		buttons.push_back(
			new Button(7 + (20 + 10)*i, 30, 20, 20, s.c_str())
		);
	}
}

void FurniturePurchasingUIView::draw() {
	theW->drawRect(0, 0, width, height, "black");
	for (int i=0, n = buttons.size(); i < n; i++) {
		Button *b = buttons[i];
		theW->drawRect(b->x, b->y, b->width, b->height, b->col());
	}
}

HiringUIView::HiringUIView(W *_theW, JenniferAniston &_aniston) : UIView(_theW, _aniston)
{
	buttons.push_back(new Button(7, 7, 12, 12, "close"));
	buttons.push_back(new Button(7, 30, 20, 20, "hire staff"));
}

void HiringUIView::draw() {
	theW->drawRect(0, 0, width, height, "black");
	for (int i=0, n = buttons.size(); i < n; i++) {
		Button *b = buttons[i];
		theW->drawRect(b->x, b->y, b->width, b->height, b->col());
	}
}
