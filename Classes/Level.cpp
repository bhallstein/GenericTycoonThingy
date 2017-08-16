#include "Level.hpp"
#include "Game.hpp"
#include "ResponderMap.hpp"
#include "NavMap.hpp"
#include "Building.hpp"
#include "Furnishing.hpp"
#include "Placeable.hpp"
#include "Unit.hpp"
#include "LuaHelper.hpp"
#include "Callback.hpp"
#include "OgreTimer.h"

Level::Level(Game *_game, W *_theW, std::string levelpath) : GameState(_game, _theW)
{
	currentlyEditedBuilding = NULL;
	levelview = NULL;
	uibarview = NULL;
	furnishingPurchasingView = NULL;
	hiringUIView = NULL;
	helpView = NULL;

	buildLevel(levelpath);
	
	JenniferAniston aniston(theW, BOTTOM_LEFT, PFIXED, PPROPORTIONAL, 0, 0, 1, 0.1);
	uibarview = new UIBarView(theW, aniston, &responderMap, &money);
	responderMap.addResponder(uibarview);
	uibarview->subscribeToButtons(new Callback(&Level::receiveEvent, this));
	
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
	responderMap.subscribeToKey(this, Event::K_H);
	
	// Time
	framecount = 0;
	timeRemaining = 0;
	realtimetime = 0.0;
	realtimetimer = new Ogre::Timer();
	
	openHelpView();

	theW->playBGM("level.mod");
}
Level::~Level()
{
	std::cout << "level destruct" << std::endl;
	destroyAllThings();
	delete navmap;
	delete levelview;
	delete uibarview;
	delete furnishingPurchasingView;
}

void Level::buildLevel(std::string levelname) {
	theW->log("Level::buildLevel() called...");
	LuaHelper mrLua(theW);
	
	std::string path = theW->luaPath;
	path.append(levelname);
	
	if (!mrLua.loadFile(path.c_str()))
		throw MsgException("Could not read level file.");	// To get error from Mr Lua: mrLua.to<std::string>(-1).c_str()
	lua_State *L = mrLua.LuaInstance;
	
	// Initialize TLO classes
	if (!Building::initialize(theW))   throw MsgException("Couldn't read building info.");
	if (!Unit::initialize(theW))       throw MsgException("Couldn't read unit info.");
	if (!Furnishing::initialize(theW)) throw MsgException("Couldn't read furnishing info.");
	if (!Behaviour::initialize(theW))  throw MsgException("Couldn't read behaviour info.");
	
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

	//set victory conditions and other level stuff (e.g. time limit)
	try {
		moneyLimit = mrLua.getvalue<int>("moneyLimit");
	} catch (MsgException &exc) {
		std::string s = "Couldn't get level's money target: ";
		s.append(exc.msg);
		throw MsgException(s.c_str());
	}
	try {
		timeLimit = mrLua.getvalue<int>("timeLimit");
	} catch (MsgException &exc) {
		std::string s = "Couldn't get level's time limit: ";
		s.append(exc.msg);
		throw MsgException(s.c_str());
	}
	
	// Create map
	navmap = new NavMap(w, h);
	levelResponderMap = new ResponderMap(w, h);
	
	// Create levelview
	JenniferAniston aniston(theW, TOP_LEFT, PFIXED, PPROPORTIONAL, 0, 0, 1, 1);
	levelview = new LevelView(theW, aniston, levelResponderMap, &buildings, &furnishings, &units, &staff, w, h, &timeRemaining);
	responderMap.addResponder(levelview);
	
	// Get allowed buildings
	try {
		mrLua.pushtable("allowedBuildings");
		lua_pushnil(L);									// S: -1 nil; -2 table
		int n = 0;
		std::string s = "  allowedBuildings: ";
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
		if (n > 0)
			s.erase(s.size() - 2);
		W::log(s.c_str());
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
					if (strcmp(dir, "upward") == 0)         d.orientation = Direction::UPWARD;
					else if (strcmp(dir, "downward") == 0)  d.orientation = Direction::DOWNWARD;
					else if (strcmp(dir, "leftward") == 0)  d.orientation = Direction::LEFTWARD;
					else if (strcmp(dir, "rightward") == 0) d.orientation = Direction::RIGHTWARD;
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
	realtimetime += realtimetimer->getMicroseconds() / 1000000.;
	realtimetimer->reset();
	timeRemaining = timeLimit - (int) realtimetime;

	intcoord c;
	for (int i=0, n = spawnPoints.size(); i < n; i++)
		if (spawnPoints[i]->spawn(&c)) {
			Unit *u = createUnit(c.x, c.y, "civilian");
			int q = W::randUpTo(3);
			if (q == 0)
				createBehaviour("despawn")->init(u);
			else if (q == 1)
				createBehaviour("seek:haircut")->init(this, u);
			else
				createBehaviour("seek:pie")->init(this, u);
		}
	
	// Update TLOs
	for (int i=0, n = units.size(); i < n; i++)      units[i]->update();
	for (int i=0, n = staff.size(); i < n; i++)      staff[i]->update();
	for (int i=0, n = buildings.size(); i < n; i++)  buildings[i]->update();
	for (int i=0, n = behaviours.size(); i < n; i++) behaviours[i]->update();
	
	destroyThings();	// Removed destroyed objects.

	//check for level end
	if((int) realtimetime >= timeLimit || money >= moneyLimit)
	{
		if((int) realtimetime >= timeLimit)
			levelScore = new LevelScore(game, theW, false);
		
		if(money >= moneyLimit)
			levelScore = new LevelScore(game, theW, true);

		game->pushState(levelScore);
	}
	
}
void Level::draw() {
	levelview->_draw();
	uibarview->_draw();
	if (furnishingPurchasingView != NULL) furnishingPurchasingView->_draw();
	if (hiringUIView != NULL) hiringUIView->_draw();
	if (helpView != NULL) helpView->_draw();
}
void Level::setResolution(int _w, int _h) {
	GameState::setResolution(_w, _h);
	levelview->updatePosition();
	uibarview->updatePosition();
}

void Level::receiveEvent(Event *ev) {
	if (ev->type == Event::SCREENEDGE_LEFT)        levelview->scroll(Direction::LEFTWARD);
	else if (ev->type == Event::SCREENEDGE_RIGHT)  levelview->scroll(Direction::RIGHTWARD);
	else if (ev->type == Event::SCREENEDGE_TOP)    levelview->scroll(Direction::UPWARD);
	else if (ev->type == Event::SCREENEDGE_BOTTOM) levelview->scroll(Direction::DOWNWARD);
	else if (ev->type == Event::KEYPRESS) {
		if (ev->key == Event::K_Q)        game->stateFinished(this, Returny(Returny::killer_returny));
		else if (ev->key == Event::K_ESC) game->stateFinished(this, Returny(Returny::empty_returny));
		else if (ev->key == Event::K_H)   openHelpView();
	}
	else if (ev->type == Event::BUTTONCLICK) {
		if (ev->payload == "open hiring ui view")                      openHiringView();
		else if (ev->payload == "close hiring ui view")                closeHiringView();
		else if (ev->payload == "close furnishing purchasing ui view") closeFurnishingPurchasingView();
		else if (ev->payload == "close help view")                     closeHelpView();
		else if (ev->payload == "hire staff") hireStaff("staff");
		else if (ev->payload == "buy furnishing barberschair") purchaseFurnishing("barberschair");
		else if (ev->payload == "buy furnishing sofa")         purchaseFurnishing("sofa");
		else if (ev->payload == "buy furnishing piecounter")   purchaseFurnishing("piecounter");
	}
}

void Level::handleCloseEvent() {
	GameState::handleCloseEvent();		// i.e. just quit for now
}

Unit* Level::createUnit(int atX, int atY, const char *type) {
	Unit *u;
	std::vector<Unit*> *vctr;
	bool initsuccess;
	if (strcmp(type, "staff") == 0) {
		u = new Unit(levelResponderMap, navmap, type, this, true);
		vctr = &staff;
		initsuccess = u->init(0, 0);
	}
	else {
		u = new Unit(levelResponderMap, navmap, type, this, false);
		vctr = &units;
		initsuccess = u->init(atX, atY);
	}
	if (initsuccess) {
		vctr->push_back(u);
		char s[50]; sprintf(s, "added unit %p of type '%s' (now %ld)", u, type, vctr->size());
		std::cout << s << std::endl;
		return u;
	}
	return NULL;
}
Building* Level::createBuilding(int atX, int atY, const char *type, std::vector<intcoord> *groundplan, std::vector<door> *doors) {
	Building *b = new Building(levelResponderMap, navmap, type, groundplan, doors, this);
	if (b->init(atX, atY)) {
		buildings.push_back(b);
		char s[50]; sprintf(s, "added building %p of type '%s' (now %ld)", b, type, buildings.size());
		std::cout << s << std::endl;
		return b;
	}
	return NULL;
}
Furnishing* Level::createFurnishing(const char *type) {
	Furnishing *f = new Furnishing(levelResponderMap, navmap, type, this, currentlyEditedBuilding);
	if (f->init(-100, -100)) {
		furnishings.push_back(f);
		std::cout << "added furnishing " << f << " (now " << furnishings.size() << ")" << std::endl;
		return f;
	}
	return NULL;
}
Behaviour* Level::createBehaviour(const char *_type) {
	Behaviour *bhvr = new Behaviour(_type, levelResponderMap);
	behaviours.push_back(bhvr);
	return bhvr;
}
void Level::purchaseFurnishing(const char *type) {
	if (Furnishing::costForType(type) <= money)
		createFurnishing(type);
	else
		std::cout << "not enough money to buy a " << type << std::endl;
}
void Level::hireStaff(const char *type) { 
	if(Unit::hireCostForType(type) <= money)
		createUnit(0, 0, type);
	else
		std::cout << "not enough money to hire a " << type << std::endl;
}

void Level::destroyThings() {
	for (std::vector<Furnishing*>::iterator i = furnishings.begin(); i < furnishings.end(); )
		if ((*i)->destroyed) {
			delete *i;
			i = furnishings.erase(i);
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
	for (std::vector<Behaviour*>::iterator i = behaviours.begin(); i < behaviours.end(); )
		if ((*i)->destroyed()) {
			delete *i;
			i = behaviours.erase(i);
		}
		else i++;
}
void Level::destroyAllThings() {
	for (int i=0; i < furnishings.size(); i++) furnishings[i]->destroyed = true;
	for (int i=0; i < buildings.size(); i++)   buildings[i]->destroyed = true;
	for (int i=0; i < units.size(); i++)       units[i]->destroyed = true;
	for (int i=0; i < behaviours.size(); i++)  behaviours[i]->destroy();
	destroyThings();
}

Building* Level::randomBuildingWithType(const char *_type) {
	int nOfType = 0, indices[50];
	for (int i=0, n = buildings.size(); i < n; i++)
		if (buildings[i]->type.compare(_type) == 0)
			indices[nOfType++] = i;
	return nOfType == 0 ? NULL : buildings[indices[W::randUpTo(nOfType)]];
}
Building* Level::buildingAtLocation(int x, int y) {
	for (int i=0, n = buildings.size(); i < n; i++) {
		Building *b = buildings[i];
		if (b->overlapsWith(x, y))
			return b;
	}
	return NULL;
}

void Level::openFurnishingPurchasingView(Building *b) {
	if (currentlyEditedBuilding == b) return;
	closeFurnishingPurchasingView();
	currentlyEditedBuilding = b;
	JenniferAniston aniston(theW, TOP_LEFT, PFIXED, PFIXED, 47, 47, 140, 220);
	furnishingPurchasingView = new FurnishingPurchasingUIView(theW, aniston, &responderMap, b->b_allowedFurnishings);
	responderMap.addResponder(furnishingPurchasingView);
	furnishingPurchasingView->subscribeToButtons(new Callback(&Level::receiveEvent, this));
}
void Level::closeFurnishingPurchasingView() {
	responderMap.removeResponder(furnishingPurchasingView);
	delete furnishingPurchasingView;
	furnishingPurchasingView = NULL;
	currentlyEditedBuilding = NULL;
}
void Level::openHiringView() {
	if (hiringUIView != NULL) return;
	JenniferAniston aniston(theW, TOP_LEFT, PFIXED, PFIXED, 10, 320, 140, 200);
	hiringUIView = new HiringUIView(theW, aniston, &responderMap);
	responderMap.addResponder(hiringUIView);
	hiringUIView->subscribeToButtons(new Callback(&Level::receiveEvent, this));
}
void Level::closeHiringView() {
	responderMap.removeResponder(hiringUIView);
	delete hiringUIView;
	hiringUIView = NULL;
}
void Level::openHelpView() {
	if (helpView != NULL) return;
	JenniferAniston aniston(theW, TOP_LEFT, PFIXED, PFIXED, 140, 77, 520, 400);
	helpView = new GTTHelpView(theW, aniston, &responderMap);
	responderMap.addResponder(helpView);
	helpView->subscribeToButtons(new Callback(&Level::receiveEvent, this));
}
void Level::closeHelpView() {
	responderMap.removeResponder(helpView);
	delete helpView;
	helpView = NULL;
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
	std::vector<Building*> *_buildings, std::vector<Furnishing*> *_furnishings, std::vector<Unit*> *_units, std::vector<Unit*> *_staff,
	int _level_width, int _level_height,
	int *_time_remaining
) :
	View(_theW, _aniston),
	levelResponderMap(_levelRM), buildings(_buildings), furnishings(_furnishings), units(_units), staff(_staff), 
	level_width(_level_width), level_height(_level_height),
	scroll_x(0), scroll_y(0), time_remaining(_time_remaining)
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
		theW->drawRect(atX + c.x*gridsize, atY + c.y*gridsize, gridsize, gridsize, col, mo->rotation);
	}
}

void LevelView::draw() {
	theW->drawRect(0, 0, width, height, "black");
	for (int i=0, n = buildings->size(); i < n; i++)   drawMappedObj((*buildings)[i]);
	for (int i=0, n = furnishings->size(); i < n; i++) drawMappedObj((*furnishings)[i]);
	for (int i=0, n = units->size(); i < n; i++)       drawMappedObj((*units)[i]);
	for (int i=0, n = staff->size(); i < n; i++)       drawMappedObj((*staff)[i]);
	char s[100];
	int time_minutes = *time_remaining / 60;
	int time_seconds = *time_remaining%60;

	sprintf(s, "%02d:%02d", time_minutes, time_seconds);
	theW->drawText(10, 10, *time_remaining <= 20 ? "red" : "white", s);
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

void LevelView::scroll(Direction::Enum dir) {
	int scrolldist = 10;
	
	if (dir == Direction::UPWARD)         scroll_y -= scrolldist;
	else if (dir == Direction::DOWNWARD)  scroll_y += scrolldist;
	else if (dir == Direction::LEFTWARD)  scroll_x -= scrolldist;
	else if (dir == Direction::RIGHTWARD) scroll_x += scrolldist;
	
	if (scroll_x < 0) scroll_x = 0;
	if (scroll_y < 0) scroll_y = 0;
	
	int max_scroll_x = level_width * gridsize - width;
	int max_scroll_y = level_height * gridsize - height;
	if (scroll_x >= max_scroll_x) scroll_x = max_scroll_x;
	if (scroll_y >= max_scroll_y) scroll_y = max_scroll_y;
}


UIBarView::UIBarView(W *_theW, JenniferAniston &_aniston, ResponderMap *_rm, int *_econ) :
	UIView(_theW, _aniston, _rm, DISALLOW_DRAG), economy(_econ)
{
	buttons.push_back(new Button(10, 10, 20, 20, "open hiring ui view"));
}
void UIBarView::draw() {
	theW->drawRect(0, 0, width, height, "black");
	for (int i=0, n = buttons.size(); i < n; i++) {
		Button *b = buttons[i];
		theW->drawRect(b->x, b->y, b->width, b->height, b->col());
	}
	char econ[10];
	sprintf(econ, "%c%d", MR_CURRENCY, *economy);
	theW->drawText(790, 10, "white", econ, true);
}


FurnishingPurchasingUIView::FurnishingPurchasingUIView(W *_theW, JenniferAniston &_aniston, ResponderMap *_rm, std::vector<std::string> *_furnishingTypes) :
	UIView(_theW, _aniston, _rm, ALLOW_DRAG), furnishingTypes(_furnishingTypes)
{
	buttons.push_back(new Button(7, 7, 12, 12, "close furnishing purchasing ui view"));
	// Add buttons for creating furnishing
	for (int i=0, n = furnishingTypes->size(); i < n; i++) {
		std::string s("buy furnishing ");
		s += furnishingTypes->at(i);
		buttons.push_back(
			new Button(7 + (20 + 10)*i, 30, 20, 20, s.c_str())
		);
	}
}
void FurnishingPurchasingUIView::draw() {
	theW->drawRect(0, 0, width, height, "black");
	for (int i=0, n = buttons.size(); i < n; i++) {
		Button *b = buttons[i];
		theW->drawRect(b->x, b->y, b->width, b->height, b->col());
	}
}

HiringUIView::HiringUIView(W *_theW, JenniferAniston &_aniston, ResponderMap *_rm) : UIView(_theW, _aniston, _rm, ALLOW_DRAG)
{
	buttons.push_back(new Button(7, 7, 12, 12, "close hiring ui view"));
	buttons.push_back(new Button(7, 30, 20, 20, "hire staff"));
}
void HiringUIView::draw() {
	theW->drawRect(0, 0, width, height, "black");
	for (int i=0, n = buttons.size(); i < n; i++) {
		Button *b = buttons[i];
		theW->drawRect(b->x, b->y, b->width, b->height, b->col());
	}
}

GTTHelpView::GTTHelpView(W *_theW, JenniferAniston &_aniston, ResponderMap *_rm) : UIView(_theW, _aniston, _rm, ALLOW_DRAG)
{
	buttons.push_back(new Button(7, 7, 12, 12, "close help view"));
}
void GTTHelpView::draw() {
	theW->drawRect(0, 0, width, height, "black");
	for (int i=0, n = buttons.size(); i < n; i++) {
		Button *b = buttons[i];
		theW->drawRect(b->x, b->y, b->width, b->height, b->col());
	}
//	theW->drawText(<#float x#>, <#float y#>, <#const char *col#>, <#char *text#>)
}

