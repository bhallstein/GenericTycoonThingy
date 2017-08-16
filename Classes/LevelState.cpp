#include "LevelState.hpp"
#include "LevelView.hpp"
#include "LevelMap.hpp"
#include "MrPaths.hpp"
#include "MrKlangy.hpp"

#include "TLO.hpp"
#include "Building.hpp"
#include "Furnishing.hpp"
#include "Unit.hpp"
#include "Controller.hpp"

LevelState::LevelState() :
	levelView(NULL),
	levelMap(NULL),
	currentlyEditedBuilding(NULL),
	paused(false)
{
	// Initialize views
	levelView = new LevelView();
	addView(levelView);
	
	// Create map
	levelMap = new LevelMap(this, levelView);
	
	// Time
	framecount = 0;
	timeRemaining = timeLimit;
	realtimetime = 0.0;
	realtimetimer = new W::Timer();
	
	MrKlangy::playBGM("level.mod");
	
	W::Messenger::subscribeToEventType(W::EventType::KEYUP, W::Callback(&LevelState::keyEvent, this));
}
LevelState::~LevelState()
{
	removeView(levelView);
	delete levelView;
	
	delete levelMap;
	
	MrKlangy::stopBGM();
}

W::EventPropagation::T LevelState::keyEvent(W::Event *ev) {
	if (ev->key == W::KeyCode::_S) saveLevel("a save game");
	else if (ev->key == W::KeyCode::ESC) W::popState(W::EmptyReturny);
	else if (ev->key == W::KeyCode::_Q)  W::popState(W::KillerReturny);
	return W::EventPropagation::SHOULD_CONTINUE;
}

void LevelState::update() {
	// Time
	if (paused) return;
	++framecount;
	realtimetime += realtimetimer->getMicroseconds() / 1000000.;
	realtimetimer->reset();
	timeRemaining = timeLimit - (int) realtimetime;
	
	// Update
	if (levelMap)
		levelMap->update();
}
void LevelState::resume(W::Returny *ret) {
	if (ret->type == W::ReturnyType::PAYLOAD_RETURNY) {
		if (ret->payload == "exit to menu") W::popState(W::EmptyReturny);
		else if (ret->payload == "replay")  W::popState(*ret);
	}
}

void LevelState::pause() {
	paused = true;
}
void LevelState::unpause() {
	realtimetimer->reset();
	paused = false;
}

bool LevelState::addPlayerMoneys(int x) {
	if (playerMoneys + x < 0) return false;
	playerMoneys += x;
	return true;
}

bool LevelState::loadLevel(const std::string &levelName) {
	using std::string;
	
	W::log << "LevelState: loading level '" << levelName << "'..." << std::endl;
	
	// Game entity class initialization
	// - loads type info
	// - creates serialization descriptors
	TLO::initialize();
	if (!Building::initialize())   { W::log << "Couldn't initialize Building class"   << std::endl; return false; }
	if (!Unit::initialize())       { W::log << "Couldn't initialize Unit class"       << std::endl; return false; }
	if (!Furnishing::initialize()) { W::log << "Couldn't initialize Furnishing class" << std::endl; return false; }
	Controller::initialize();
	CustomerController::initialize();
	
	// Load level lua file
	string path = MrPaths::resourcesPath + string("Data/Levels/") + levelName + ".lua";
	lua_State *L;
	if (!luaLoad(path, &L)) {
		W::log << "Couldn't load file '" << path << "'" << std::endl;
		return false;
	}
	
	// Get LuaObjs from file
	lua_getglobal(L, "levelData");   LuaObj levelDataObj(L);
	lua_getglobal(L, "playerState"); LuaObj playerStateObj(L);
	lua_getglobal(L, "mapData");     LuaObj mapDataObj(L);
	lua_close(L);
	
	if (!levelDataObj.isTable()) {
		W::log << "Error: levelData table not found" << std::endl;
		return false;
	}
	if (!playerStateObj.isTable()) {
		W::log << "Error: playerState table not found" << std::endl;
		return false;
	}
	if (!mapDataObj.isTable()) {
		W::log << "Error: mapData table not found" << std::endl;
		return false;
	}
	
	// Level Data
	LuaObj &monetaryTargetObj = levelDataObj["monetaryTarget"];
	if (!monetaryTargetObj.isNumber()) {
		W::log << "levelData.monetaryTarget not found or not number" << std::endl;
		return false;
	}
	monetaryTarget = monetaryTargetObj.number_value;
	LuaObj &timeLimitObj = levelDataObj["timeLimit"];
	if (!timeLimitObj.isNumber()) {
		W::log << "levelData.timeLimit not found or not number" << std::endl;
		return false;
	}
	timeLimit = timeLimitObj.number_value;
	
	// Player State
	LuaObj &balanceObj = playerStateObj["balance"];
	if (!balanceObj.isNumber()) {
		W::log << "playerState.balance not found or not number" << std::endl;
		return false;
	}
	playerMoneys = balanceObj.number_value;
	
	// Map contents
	bool mapload_success = levelMap->load(mapDataObj);
	// Note: since the level's size info is loaded by levelMap,
	// levelMap also calls levelView.setLevelSize, and creates the NavMap.
	if (!mapload_success) {
		W::log << "Error loading map" << std::endl;
		return false;
	}
	
	W::log << "...loaded." << std::endl;	
	return true;
}

bool LevelState::saveLevel(const std::string &saveName) {
	using std::string;
	std::stringstream ss;
	
	// Title
	ss << "-- " << saveName << "\n\n";
	
	// Level Data
	ss << "levelData = {\n";
	ss << "monetaryTarget = " << monetaryTarget << ",\n";
	ss << "timeLimit = " << timeLimit << "\n";
	ss << "}\n\n";
	
	// Player State
	ss << "playerState = {\n";
	ss << "balance = " << playerMoneys << "\n";
	ss << "}\n\n";
	
	// Map contents
	ss << "mapData = {\n";
	ss << levelMap->save();
	ss << "}\n";
	
	// Save file to save games directory
	string saveDir = MrPaths::settingsPath + "Saved games/";
	string saveFileName = saveDir + saveName + ".lua";
	if (!W::isValidDir(MrPaths::settingsPath))
		if (!W::createDir(MrPaths::settingsPath)) return false;
	if (!W::isValidDir(saveDir))
		if (!W::createDir(saveDir)) return false;
	std::ofstream f(saveFileName.c_str());
	std::string s = ss.str();
	reindentLuaString(s);
	f << s;
	f.close();
	
	return true;
}
