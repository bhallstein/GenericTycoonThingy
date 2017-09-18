/*
 * Generic Tycoon Thingy
 *
 * ==================
 *  LevelState.cpp
 * ==================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

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
#include "W.h"

LevelState::LevelState() :
	levelView(NULL),
	levelMap(NULL),
	paused(false)
{
	// Initialize views
	levelView = new LevelView();
	addView(levelView);
	
	// Create map
	levelMap = new LevelMap(this, levelView);
	
	// Time
	realtimetime = 0.0;
	realtimetimer = new W::Timer();
	
//	MrKlangy::playBGM("level.mod");
	
	W::Messenger::subscribe(W::EventType::KeyUp, W::Callback(&LevelState::keyEvent, this));
}
LevelState::~LevelState()
{
	removeView(levelView);
	delete levelView;
	
	delete levelMap;
	
//	MrKlangy::stopBGM();
}

W::EventPropagation::T LevelState::keyEvent(W::Event *ev) {
	if (ev->key == W::KeyCode::_S) { saveLevel("a save game " + std::to_string(W::Rand::intUpTo(10000000))); }
	else if (ev->key == W::KeyCode::ESC) { W::popState(W::EmptyReturny); }
	else if (ev->key == W::KeyCode::_Q)  { W::popState(W::KillerReturny); }
	else if (ev->key == W::KeyCode::_P) {
		if (paused) { unpause(); }
		else { pause(); }
	}
	return W::EventPropagation::ShouldContinue;
}

void LevelState::update() {
	// Time
	if (paused) {
		return;
	}
	
	int microseconds_elapsed = realtimetimer->getMicroseconds();
	if (microseconds_elapsed > 100000) {
		microseconds_elapsed = 100000;
	}
	
	realtimetime += microseconds_elapsed / 1000000.0;
	realtimetimer->reset();
	
	// Update
	if (levelMap) {
		levelMap->update(microseconds_elapsed, realtimetime);
	}
}
void LevelState::resume(W::Returny *ret) {
	if (ret->type == W::ReturnyType::Payload) {
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
	ShopkeeperController::initialize();
	
	// Load level lua file
	string path = MrPaths::resourcesPath + string("Data/Levels/") + levelName + ".lua";
	lua_State *L;
	if (!luaLoad(path, &L)) {
		W::log << "Couldn't load file '" << path << "'" << std::endl;
		return false;
	}
	
	// Map contents
	bool mapload_success = levelMap->load(L);
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
	
	string s = levelMap->save();
	
	// Prepend title
	s = string("-- ") + saveName + string("\n\n") + s;
	
	// Save file to save games directory
	string saveDir = MrPaths::settingsPath + "Saved games/";
	string saveFileName = saveDir + saveName + ".lua";
	if (!W::isValidDir(MrPaths::settingsPath))
		if (!W::createDir(MrPaths::settingsPath)) return false;
	if (!W::isValidDir(saveDir)) {
		bool dir_created = W::createDir(saveDir);
		if (!dir_created) {
			return false;
		}
	}
	std::ofstream f(saveFileName.c_str());
	reindentLuaString(s);
	f << s;
	f.close();
	
	return true;
}
