#include "Level.hpp"
#include "Building.hpp"
#include "Furnishing.hpp"
#include "Placeable.hpp"
#include "Unit.hpp"
#include "LuaHelper.hpp"
#include "LevelScore.hpp"
#include "MrPaths.hpp"
#include "MrKlangy.hpp"

W::EventType::T W::EventType::LEVEL_LEFTMOUSEDOWN  = W::Event::registerType();
W::EventType::T W::EventType::LEVEL_RIGHTMOUSEDOWN = W::Event::registerType();
W::EventType::T W::EventType::LEVEL_LEFTMOUSEUP    = W::Event::registerType();
W::EventType::T W::EventType::LEVEL_RIGHTMOUSEUP   = W::Event::registerType();
W::EventType::T W::EventType::LEVEL_MOUSEMOVE      = W::Event::registerType();

Level::Level(W::Window *_win, std::string levelpath) : win(_win)
{
	currentlyEditedBuilding = NULL;
	levelview = NULL;
	uibarview = NULL;
	furnishingPurchasingView = NULL;
	hiringUIView = NULL;
	helpView = NULL;
	
	paused = false;

	buildLevel(levelpath);
	
	uibarview = new UIBarView(win, &money);
	addView(uibarview);
	uibarview->subscribeToButtons(new W::Callback(&Level::buttonEvent, this));
	
	// Subscribe to screenedge events
	W::Callback c(&Level::scrollEvent, this);
	W::Messenger::subscribeToEventType(W::EventType::SCREENEDGE_LEFT, c);
	W::Messenger::subscribeToEventType(W::EventType::SCREENEDGE_RIGHT, c);
	W::Messenger::subscribeToEventType(W::EventType::SCREENEDGE_TOP, c);
	W::Messenger::subscribeToEventType(W::EventType::SCREENEDGE_BOTTOM, c);
	
	// Key subscriptions
	W::Messenger::subscribeToEventType(W::EventType::KEYPRESS, W::Callback(&Level::keyEvent, this));
	
	// Time
	framecount = 0;
	timeRemaining = timeLimit;
	realtimetime = 0.0;
	realtimetimer = new Ogre::Timer();
	
	spawnRate = 0;
	
	openHelpView();

	MrKlangy::playBGM("level.mod");
}
Level::~Level()
{
	std::cout << "level destruct" << std::endl;
	destroyAllThings();
	
	removeView(levelview); delete levelview;
	removeView(uibarview); delete uibarview;
	closeFurnishingPurchasingView();
	closeHiringView();
	closeHelpView();

	delete navmap;
	
	MrKlangy::stopBGM();
}

void Level::buildLevel(std::string levelname) {
	W::log << "Level::buildLevel() called..." << std::endl;
	LuaHelper mrLua;
	
	std::string path = MrPaths::luaPath;
	path.append(levelname);
	
	if (!mrLua.loadFile(path.c_str()))
		throw W::Exception("Could not read level file.");	// To get error from Mr Lua: mrLua.to<std::string>(-1).c_str()
	lua_State *L = mrLua.LuaInstance;
	
	// Initialize TLO classes
	if (!Building::initialize())   throw W::Exception("Couldn't read building info.");
	if (!Unit::initialize())       throw W::Exception("Couldn't read unit info.");
	if (!Furnishing::initialize()) throw W::Exception("Couldn't read furnishing info.");
	if (!Behaviour::initialize())  throw W::Exception("Couldn't read behaviour info.");
	
	// Set level width and height
	try {
		w = mrLua.getvalue<int>("width");
		h = mrLua.getvalue<int>("height");
	} catch (W::Exception &exc) {
		std::string s = "Couldn't get level's dimensions: ";
		throw W::Exception((s + exc.what()).c_str());
	}

	// Set level (player's) money to the starting balance
	try {
		money = mrLua.getvalue<int>("startingBalance");
	} catch (W::Exception &exc) {
		std::string s = "Couldn't get level's starting balance: ";
		throw W::Exception((s + exc.what()).c_str());
	}

	// Set victory conditions and other level stuff (e.g. time limit)
	try {
		moneyLimit = mrLua.getvalue<int>("moneyLimit");
	} catch (W::Exception &exc) {
		std::string s = "Couldn't get level's money target: ";
		throw W::Exception((s + exc.what()).c_str());
	}
	try {
		timeLimit = mrLua.getvalue<int>("timeLimit");
	} catch (W::Exception &exc) {
		std::string s = "Couldn't get level's time limit: ";
		throw W::Exception((s + exc.what()).c_str());
	}
	
	// Create map
	navmap = new W::NavMap(w, h);
	
	// Create levelview
	levelview = new LevelView(win, &buildings, &furnishings, &units, &staff, w, h, &timeRemaining, navmap);
	addView(levelview);
	
	// Get allowed buildings
	try {
		mrLua.pushtable("allowedBuildings");
		lua_pushnil(L);									// S: nil, table
		W::log << "  Allowed building types:" << std::endl;
		while (lua_next(L, -2) != 0) {					// S: val, key, table
			if (!lua_isstring(L, -1)) {
				lua_pop(L, 1);
				continue;
			}
			allowedBuildings.push_back(lua_tostring(L, -1));
			W::log << "    " << allowedBuildings.back() << std::endl;
			lua_pop(L, 1);								// S: key, table
		}
		lua_settop(L, 0);	// S: ~
	} catch (W::Exception &exc) {
		std::string s = "Error getting list of allowed buildings for level: ";
		throw W::Exception((s + exc.what()).c_str());
	}
	
	// Add buildings
	W::log << "  Adding buildings..." << std::endl;
	if (mrLua.pushtable("buildings")) {	// S: buildings
		try {
			lua_pushnil(L);						// S: nil, buildings
			while (lua_next(L, -2) != 0) {		// S: building, key, buildings
				if (lua_type(L, -1) != LUA_TTABLE) {
					lua_pop(L, 1);
					continue;
				}
				// Set position & type
				W::position bpos(mrLua.getSubfield<int>("position", 1), mrLua.getSubfield<int>("position", 2));
				const char *bType = mrLua.getfield<const char *>("type");
				
				// Set ground plan
				std::vector<W::rect> bPlan;
				if (!mrLua.pushSubtable("plan")) throw W::Exception("Could not find plan table.");
				lua_pushnil(L);									// S: nil, plan, building, key, buildings
				for (int i=0; lua_next(L, -2); i++) {			// S: rect, key, plan, ...
					if (!lua_istable(L, -1)) {
						lua_pop(L, 1);
						continue;
					}
					W::rect r = {
						W::position(mrLua.getSubfield<int>("pos", 1), mrLua.getSubfield<int>("pos", 2)),
						W::size(mrLua.getSubfield<int>("sz", 1), mrLua.getSubfield<int>("sz", 2))
					};
					bPlan.push_back(r);
					lua_pop(L, 1);								// S: key, plan, building, key, buildings
				}												// S: plan, building, key, buildings
				lua_pop(L, 1);									// S: building, key, building
				lua_pop(L, 1);									// S: key, buildings
				
				W::log << "    " << bType << " at " << bpos.x << "," << bpos.y << " rects:";
				for (std::vector<W::rect>::iterator it = bPlan.begin(); it < bPlan.end(); it++)
					W::log << " " << it->pos.x << "," << it->pos.y << "->" << it->pos.x + it->sz.width << "," << it->pos.y + it->sz.height;
				W::log << std::endl;
				createBuilding(bpos, bType, &bPlan);	// TODO: what if building can't be added?
			}
			lua_settop(L, 0);	// S: ~
		} catch (W::Exception &exc) {
			std::string s = "Couldn't load buildings: ";
			throw W::Exception((s + exc.what()).c_str());
		}
	}
	if (buildings.size() == 0)
		W::log << "    no buildings found" << std::endl;
	
	// Add furnishings
	W::log << "  Adding furnishings..." << std::endl;
	if (mrLua.pushtable("furnishings")) {		// S: furnishings
		try {
			lua_pushnil(L);						// S: nil, furnishings
			while (lua_next(L, -2) != 0) {		// S: furnishing, key, furnishings
				if (!lua_istable(L, -1)) {
					lua_pop(L, 1);
					continue;
				}
				// Set position & type
				W::position bpos(mrLua.getSubfield<int>("position", 1), mrLua.getSubfield<int>("position", 2));
				const char *fType = mrLua.getfield<const char *>("type");
				
				W::log << "    " << fType << " at " << bpos.x << "," << bpos.y << std::endl;
				createFurnishing(fType, false, bpos.x, bpos.y);
				lua_pop(L, 1);					// S: key, furnishings
			}									// S: furnishings
			lua_settop(L, 0);	// S: ~
		} catch (W::Exception &exc) {
			std::string s = "Couldn't load furnishings: ";
			throw W::Exception((s + exc.what()).c_str());
		}
	}
	if (furnishings.size() == 0)
		W::log << "    no furnishings found" << std::endl;
	
	// Add spawn points
	W::log << "  Adding spawn points..." << std::endl;
	if (mrLua.pushtable("spawnPoints")) {
		try {
			lua_pushnil(L);						// S: nil, table
			while (lua_next(L, 1) != 0) {		// S: val, key, table
				spawnPoints.push_back(new SpawnPoint(
					mrLua.getSubfield<int>("pos", 1),
					mrLua.getSubfield<int>("pos", 2)
	//				mrLua.getfield<const char *>("name"),
	//				mrLua.getfield<int>("rate")
				));
				W::log << "    at " << spawnPoints.back()->x << "," << spawnPoints.back()->y << std::endl;
				lua_pop(L, 1);					// S: key, table
			}
		} catch (W::Exception &exc) {
			std::string s = "Couldn't add spawn points: ";
			throw W::Exception((s + exc.what()).c_str());
		}
	}
	if (spawnPoints.size() == 0)
		W::log << "    no spawn points found" << std::endl;
}

void Level::pause() {
	paused = true;
}
void Level::unpause() {
	realtimetimer->reset();
	paused = false;
}
void Level::resume(W::Returny *ret) {
	printf("Level::resume\n");
	if (ret->type == W::ReturnyType::PAYLOAD_RETURNY) {
		if (ret->payload == "exit to menu") W::popState(W::EmptyReturny);
		else if (ret->payload == "replay")  W::popState(*ret);
	}
}

void Level::update() {
	if (paused) return;
	
	framecount++;
	realtimetime += realtimetimer->getMicroseconds() / 1000000.;
	realtimetimer->reset();
	timeRemaining = timeLimit - (int) realtimetime;
	
	if (framecount == 5) {
		W::position p;
		spawnPoints[0]->getCoords(&p);
		Unit *u = createUnit(p.x, p.y, "customer");
		addBehaviour(new CustomerBehaviour(u, this));
	}
//	float some_coefficient = 0.2;
//	float spawnRate = realtimetime / timeLimit * some_coefficient;
//	if (W::randUpTo(100) < spawnRate * 100) {
//		W::position p;
//		SpawnPoint *sp = spawnPoints[W::randUpTo(spawnPoints.size())];
//		sp->getCoords(&p);
//		
//		Unit *u = createUnit(p.x, p.y, "customer");
//		addBehaviour(new CustomerBehaviour(&eh, u, this));
//	}
	
	// Update TLOs
	for (int i=0; i < units.size(); i++)      units[i]->update();
	for (int i=0; i < staff.size(); i++)      staff[i]->update();
	for (int i=0; i < buildings.size(); i++)  buildings[i]->update();
	for (int i=0; i < behaviours.size(); i++) behaviours[i]->update();
	
	// Remove destroyed objects
	destroyThings();
	
	// Check for win/lose
	if((int)realtimetime >= timeLimit) {
		levelScore = new LevelScore(win, false);
		W::pushState(levelScore);
	}
	else if (money >= moneyLimit) {
		levelScore = new LevelScore(win, true);
		W::pushState(levelScore);
	}
}

void Level::scrollEvent(W::Event *ev) {
	if      (ev->type == W::EventType::SCREENEDGE_LEFT)   levelview->scroll(Direction::LEFTWARD);
	else if (ev->type == W::EventType::SCREENEDGE_RIGHT)  levelview->scroll(Direction::RIGHTWARD);
	else if (ev->type == W::EventType::SCREENEDGE_TOP)    levelview->scroll(Direction::UPWARD);
	else if (ev->type == W::EventType::SCREENEDGE_BOTTOM) levelview->scroll(Direction::DOWNWARD);
}
void Level::keyEvent(W::Event *ev) {
	if      (ev->key == W::KeyCode::_Q)   W::popState(W::KillerReturny);
	else if (ev->key == W::KeyCode::_H)   openHelpView();
	else if (ev->key == W::KeyCode::ESC) {
		if (helpView != NULL) closeHelpView();
		else if (furnishingPurchasingView != NULL) closeFurnishingPurchasingView();
		else if (hiringUIView != NULL) closeHiringView();
		else W::popState(W::EmptyReturny);
	}
	else if (ev->key == W::KeyCode::_W) {
		levelScore = new LevelScore(win, true);
		W::pushState(levelScore);
	}
}

void Level::buttonEvent(W::Event *ev) {
	if      (*((std::string*)ev->_payload) == "open hiring ui view")                      openHiringView();
	else if (*((std::string*)ev->_payload) == "close hiring ui view")                closeHiringView();
	else if (*((std::string*)ev->_payload) == "close furnishing purchasing ui view") closeFurnishingPurchasingView();
	else if (*((std::string*)ev->_payload) == "close help view")                     closeHelpView();
	else if (*((std::string*)ev->_payload) == "hire staff") hireStaff("shopkeeper");
	else if (*((std::string*)ev->_payload) == "buy furnishing barberschair") purchaseFurnishing("barberschair");
	else if (*((std::string*)ev->_payload) == "buy furnishing sofa")         purchaseFurnishing("sofa");
	else if (*((std::string*)ev->_payload) == "buy furnishing piecounter")   purchaseFurnishing("piecounter");
}

void Level::handleCloseEvent() {
	W::GameState::handleCloseEvent();		// i.e. just quit for now
}

Unit* Level::createUnit(int atX, int atY, const char *type) {
	Unit *u;
	std::vector<Unit*> *vctr;
	bool initsuccess;
	bool isStaff = Unit::infoForType(type)->isStaff;
	if (isStaff) {
		u = new Unit(navmap, type, this, true);
		vctr = &staff;
		initsuccess = u->init(-100, -100);
	}
	else {
		u = new Unit(navmap, type, this, false);
		vctr = &units;
		initsuccess = u->init(atX, atY);
	}
	
	if (initsuccess) {
		vctr->push_back(u);
		printf(
			"added %sunit %p of type \"%s\" (now %ld)\n",
			isStaff ? "staff " : "", u, type, vctr->size()
		);
		return u;
	}
	// TODO: deallocate on init failure?
	return NULL;
}
Building* Level::createBuilding(W::position &pos, const char *type, std::vector<W::rect> *plan) {
	Building *b = new Building(navmap, type, plan, pos, this);
	buildings.push_back(b);
	printf("added building %p of type '%s' (now %ld)\n", b, type, buildings.size());
	return b;
}
Furnishing* Level::createFurnishing(const char *type, bool placeableMode, int atX, int atY) {
	Furnishing *f;
	std::vector<Furnishing*> *vctr;
	if (strstarts(type, "door")) {
		f = new Door(navmap, type, this, placeableMode);
		vctr = &doors;
	}
	else {
		f = new Furnishing(navmap, type, this, currentlyEditedBuilding, placeableMode);
		vctr = &furnishings;
	}
	
	bool initsuccess = (placeableMode ? f->init(-100, -100) : f->init(atX, atY));
	if (initsuccess) {
		vctr->push_back(f);
		printf("added furnishing %p of type '%s' (now %ld)\n", f, type, vctr->size());
		return f;
	}
	else {
		std::cout << "furnishing init failure" << std::endl;
	}
	// TODO: deallocate on init failure?
	return NULL;
}
void Level::addBehaviour(Behaviour *_b) {
	behaviours.push_back(_b);
}
void Level::purchaseFurnishing(const char *type) {
	if (Furnishing::costForType(type) <= money)
		createFurnishing(type);
	else
		std::cout << "not enough money to buy a " << type << std::endl;
}
void Level::hireStaff(const char *type) { 
	if(Unit::infoForType(type)->hireCost <= money) {
		Unit *u = createUnit(0, 0, type);
		addBehaviour(new ShopKeeperBehaviour(u, this));
	}
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
	for (std::vector<Behaviour*>::iterator it = behaviours.begin(); it < behaviours.end(); )
		if ((*it)->destroyed) {
			delete *it;
			it = behaviours.erase(it);
		}
		else it++;
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
	for (int i=0; i < buildings.size(); i++)
		if (buildings[i]->type.compare(_type) == 0)
			indices[nOfType++] = i;
	return nOfType == 0 ? NULL : buildings[indices[W::randUpTo(nOfType)]];
}
Building* Level::buildingAtPosition(const W::position &p) {
	for (std::vector<Building*>::iterator it = buildings.begin(); it < buildings.end(); it++)
		if ((*it)->overlapsWith(p))
			return *it;
	return NULL;
}

void Level::openFurnishingPurchasingView(Building *b) {
	if (b == currentlyEditedBuilding) return;
	closeFurnishingPurchasingView();
	currentlyEditedBuilding = b;
	furnishingPurchasingView = new FurnishingPurchasingUIView(win, &b->bInfo->allowedFurnishings);
	addView(furnishingPurchasingView);
	furnishingPurchasingView->subscribeToButtons(new W::Callback(&Level::buttonEvent, this));
}
void Level::closeFurnishingPurchasingView() {
	removeView(furnishingPurchasingView);
	delete furnishingPurchasingView;
	furnishingPurchasingView = NULL;
	currentlyEditedBuilding = NULL;
}
void Level::openHiringView() {
	if (hiringUIView != NULL) return;
	hiringUIView = new HiringUIView(win);
	addView(hiringUIView);
	hiringUIView->subscribeToButtons(new W::Callback(&Level::buttonEvent, this));
}
void Level::closeHiringView() {
	removeView(hiringUIView);
	delete hiringUIView;
	hiringUIView = NULL;
}
void Level::openHelpView() {
	if (helpView != NULL) return;
	pause();
	helpView = new GTTHelpView(win, &timeRemaining, &moneyLimit);
	addView(helpView);
	helpView->subscribeToButtons(new W::Callback(&Level::buttonEvent, this));
}
void Level::closeHelpView() {
	unpause();
	removeView(helpView);
	delete helpView;
	helpView = NULL;
}

// Money stuff
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


LevelView::LevelView(
	W::Window *_win,
	std::vector<Building*> *_buildings, std::vector<Furnishing*> *_furnishings, std::vector<Unit*> *_units, std::vector<Unit*> *_staff,
	int _level_width, int _level_height,
	int *_time_remaining,
	W::NavMap *_nm
) :
	View(
		 new W::Positioner(W::TOP_LEFT, W::PFIXED, W::PPROPORTIONAL, 0, 0, 1, 1),
		_win
	),
	buildings(_buildings), furnishings(_furnishings), units(_units), staff(_staff),
	level_width(_level_width), level_height(_level_height),
	scroll_x(0), scroll_y(0), time_remaining(_time_remaining), nm(_nm)
{
	gridsize = 20;		// Pixel size of a level block
}

void LevelView::drawTLOAt(TLO *cuthbert, W::position &pos) {
	int vwidth = plan[0].sz.width, vheight = plan[0].sz.height;
	if (pos.x >= vwidth + gridsize || pos.y >= vheight + gridsize)
		return;
	W::Colour col = cuthbert->col();
	for (std::vector<W::rect>::iterator it = cuthbert->plan.begin(); it < cuthbert->plan.end(); it++) {
		W::rect *r = &*it;
		int rx = r->pos.x, ry = r->pos.y, rwidth = r->sz.width, rheight = r->sz.height;
		drawRect(pos.x + rx*gridsize, pos.y + ry*gridsize, rwidth*gridsize, rheight*gridsize, col, cuthbert->rotation);
	}
}
void LevelView::drawTLO(TLO *jeff) {
	// Calculate position, accounting for scroll
	W::position pos(
		(int) (jeff->pos.x + jeff->pos.a) * gridsize - scroll_x,
		(int) (jeff->pos.y + jeff->pos.b) * gridsize - scroll_y
	);
	drawTLOAt(jeff, pos);
}
void LevelView::drawPM(PlaceableManager *susan) {
	W::position pos;
	if (susan->placeableMode) {
		pos.x = susan->placeable->pos.x * gridsize;
		pos.y = susan->placeable->pos.y * gridsize;
	}
	else {
		pos.x = (susan->pos.x + susan->pos.a) * gridsize;
		pos.y = (susan->pos.y + susan->pos.b) * gridsize;
	}
	pos.x -= scroll_x;
	pos.y -= scroll_y;
	drawTLOAt(susan, pos);
}

void LevelView::draw() {
	drawRect(0, 0, plan[0].sz.width, plan[0].sz.height, W::Colour::TransparentBlack);
	
	for (std::vector<Building*>::iterator it = buildings->begin(); it < buildings->end(); it++) drawTLO(*it);
	for (std::vector<Furnishing*>::iterator it = furnishings->begin(); it < furnishings->end(); it++) drawPM(*it);
	for (std::vector<Unit*>::iterator it = units->begin(); it < units->end(); it++) drawPM(*it);
	for (std::vector<Unit*>::iterator it = staff->begin(); it < staff->end(); it++) drawPM(*it);
	
	char s[50];
	int time_minutes = *time_remaining / 60;
	int time_seconds = *time_remaining%60;
	sprintf(s, "%02d:%02d", time_minutes, time_seconds);
	drawText(10, 10, *time_remaining <= 20 ? W::Colour::Red : W::Colour::White, s);

	// Draw passability from one place to another.
//	std::vector<W::position> rt;
//	int fromX = 1, fromY = 2;
//	for (int j=0; j < level_height; j++)
//		for (int i=0; i < level_width; i++)
//			drawRect(i*gridsize, j*gridsize, gridsize, gridsize, nm->getRoute(fromX, fromY, i, j, &rt) ? W::Colour::GREEN : W::Colour::RED);
	
	// Draw neighbours under mouse
//	W::NavNode *n = nm->_nodeAt(mousepos.x, mousepos.y);
//	drawRect(n->x*gridsize, n->y*gridsize, gridsize, gridsize, GREEN);
//	for (std::vector<W::NavNode*>::iterator it = n->neighbours.begin(); it < n->neighbours.end(); it++) {
//		W::NavNode *nn = *it;
//		drawRect(nn->x*gridsize, nn->y*gridsize, gridsize, gridsize, n->hasNeighbour(nn) ? GREEN : RED);
//	}
}

void LevelView::processMouseEvent(W::Event *ev) {
	// Convert event from View coordinates to grid coordinates
	ev->pos.x += scroll_x;
	ev->pos.y += scroll_y;
	ev->pos.a = (float) (ev->pos.x%gridsize) / (float) gridsize;
	ev->pos.b = (float) (ev->pos.y%gridsize) / (float) gridsize;
	ev->pos.x /= gridsize;
	ev->pos.y /= gridsize;
	// Resubmit converted event with LEVEL_X type
	using namespace W::EventType;
	switch (ev->type) {
		case LEFTMOUSEDOWN  : ev->type = LEVEL_LEFTMOUSEDOWN; break;
		case RIGHTMOUSEDOWN : ev->type = LEVEL_RIGHTMOUSEDOWN; break;
		case LEFTMOUSEUP    : ev->type = LEVEL_LEFTMOUSEUP; break;
		case RIGHTMOUSEUP   : ev->type = LEVEL_RIGHTMOUSEUP; break;
		case MOUSEMOVE      : ev->type = LEVEL_MOUSEMOVE; break;
	}
//	if (ev->type == LEVEL_LEFTMOUSEDOWN)
//		printf("lmousedown at %d,%d\n", ev->pos.x, ev->pos.y);
	W::Messenger::dispatchEvent(ev);
}

void LevelView::scroll(Direction::T dir) {
	int scrolldist = 10;
	
	if (dir == Direction::UPWARD)         scroll_y -= scrolldist;
	else if (dir == Direction::DOWNWARD)  scroll_y += scrolldist;
	else if (dir == Direction::LEFTWARD)  scroll_x -= scrolldist;
	else if (dir == Direction::RIGHTWARD) scroll_x += scrolldist;
	
	if (scroll_x < 0) scroll_x = 0;
	if (scroll_y < 0) scroll_y = 0;
	
	int max_scroll_x = level_width * gridsize - plan[0].sz.width;
	int max_scroll_y = level_height * gridsize - plan[0].sz.height;
	if (scroll_x >= max_scroll_x) scroll_x = max_scroll_x;
	if (scroll_y >= max_scroll_y) scroll_y = max_scroll_y;
}


UIBarView::UIBarView(W::Window *_win, int *_econ) :
	UIView(
		new W::Positioner(W::BOTTOM_LEFT, W::PFIXED, W::PPROPORTIONAL, 0, 0, 1, 0.1),
		_win
	),
	economy(_econ)
{
	W::rect r = {
		W::position(10, 10),
		W::size(20, 20)
	};
	buttons.push_back(new W::Button(r, "open hiring ui view"));
}
void UIBarView::draw() {
	drawRect(0, 0, plan[0].sz.width, plan[0].sz.height, W::Colour::TransparentBlack);
	for (int i=0; i < buttons.size(); i++) {
		W::Button *b = buttons[i];
		drawRect(b->pos.x, b->pos.y, b->plan[0].sz.width, b->plan[0].sz.height, b->col());
	}
	char econ[14];
	sprintf(econ, "%c%d", MR_CURRENCY, *economy);
	drawText(790, 10, W::Colour::White, econ, true);
}


FurnishingPurchasingUIView::FurnishingPurchasingUIView(W::Window *_win, std::vector<std::string> *_furnishingTypes) :
	UIView(
		new W::Positioner(W::TOP_LEFT, W::PFIXED, W::PFIXED, 47, 47, 140, 220),
		_win,
		W::ALLOW_DRAG
	),
	furnishingTypes(_furnishingTypes)
{
	W::rect r = {
		W::position(7, 7),
		W::size(12, 12)
	};
	buttons.push_back(new W::Button(r, "close furnishing purchasing ui view"));
	// Add buttons for creating furnishing
	for (int i=0; i < furnishingTypes->size(); i++) {
		std::string s("buy furnishing ");
		s += furnishingTypes->at(i);
		W::rect r = {
			W::position(7 + (20+10)*i, 30),
			W::size(20, 20)
		};
		buttons.push_back(new W::Button(r, s.c_str()));
	}
}
void FurnishingPurchasingUIView::draw() {
	drawRect(0, 0, plan[0].sz.width, plan[0].sz.height, W::Colour(1,1,1,0.5));
	for (int i=0; i < buttons.size(); i++) {
		W::Button *b = buttons[i];
		if (i == 0) {
			drawRect(b->pos.x,   b->pos.y, b->plan[0].sz.width, b->plan[0].sz.height, b->col());
			drawRect(b->pos.x+2, b->pos.y+5, 8, 2, W::Colour::White, 45);
			drawRect(b->pos.x+2, b->pos.y+5, 8, 2, W::Colour::White, -45);
		}
		else
			drawRect(b->pos.x, b->pos.y, b->plan[0].sz.width, b->plan[0].sz.height, b->col());
	}
}

HiringUIView::HiringUIView(W::Window *_win) :
	UIView(
		new W::Positioner(W::TOP_LEFT, W::PFIXED, W::PFIXED, 10, 320, 140, 200),
		_win,
		W::ALLOW_DRAG
	)
{
	W::rect r = {
		W::position(7, 7),
		W::size(12, 12)
	};
	W::rect r2 = {
		W::position(7, 30),
		W::size(20, 20)
	};
	buttons.push_back(new W::Button(r, "close hiring ui view"));
	buttons.push_back(new W::Button(r2, "hire staff"));
}
void HiringUIView::draw() {
	drawRect(0, 0, plan[0].sz.width, plan[0].sz.height, W::Colour(1,1,1,0.5));
	for (int i=0; i < buttons.size(); i++) {
		W::Button *b = buttons[i];
		if (i == 0) {
			drawRect(b->pos.x,   b->pos.y, b->plan[0].sz.width, b->plan[0].sz.height, b->col());
			drawRect(b->pos.x+2, b->pos.y+5, 8, 2, W::Colour::White, 45);
			drawRect(b->pos.x+2, b->pos.y+5, 8, 2, W::Colour::White, -45);
		}
		else
			drawRect(b->pos.x, b->pos.y, b->plan[0].sz.width, b->plan[0].sz.height, b->col());
	}
}

GTTHelpView::GTTHelpView(W::Window *_win, int *_time_remaining, int *_monetary_target) :
	UIView(
		new W::Positioner(W::TOP_LEFT, W::PFIXED, W::PFIXED, 140, 77, 520, 350),
		_win,
		W::ALLOW_DRAG
	),
	time_remaining(_time_remaining), monetary_target(_monetary_target)
{
	W::rect r = {
		W::position(7, 7),
		W::size(12, 12)
	};
	buttons.push_back(new W::Button(r, "close help view"));
}
void GTTHelpView::draw() {
	drawRect(0, 0, plan[0].sz.width, plan[0].sz.height, W::Colour::TransparentBlack);

	// Draw close button
	W::Button *b = buttons[0];
	drawRect(b->pos.x,   b->pos.y, b->plan[0].sz.width, b->plan[0].sz.height, b->col());
	drawRect(b->pos.x+2, b->pos.y+5, 8, 2, W::Colour::White, 45);
	drawRect(b->pos.x+2, b->pos.y+5, 8, 2, W::Colour::White, -45);
	
	drawText(220, 10, W::Colour::White, (char*)"Help");
	drawText(14,  54, W::Colour::White, (char*)"place furniture and staff in your");
	drawText(14,  74, W::Colour::White, (char*)"buildings. customers will come and");
	drawText(14,  94, W::Colour::White, (char*)"give you money.");
	int time_minutes = *time_remaining / 60;
	int time_seconds = *time_remaining%60;
	char s[60];
	sprintf(s, "you've got %02d:%02d left to earn %c%d", time_minutes, time_seconds, MR_CURRENCY, *monetary_target);
	drawText(14, 144, W::Colour::White, s);
	drawText(14, 184, W::Colour::White, (char*)"Get cracking! GO!");
	
	drawRect(10, 234, 500, 1, W::Colour(1, 1, 1, 0.3), 0);

	drawText(14, 264, W::Colour::White, (char*)"Esc: quit to main menu");
	drawText(14, 284, W::Colour::White, (char*)"Q: quit completely");
	drawText(14, 304, W::Colour::White, (char*)"H: help");
}
