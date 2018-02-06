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
#include "Serializer.hpp"

#include "Views__UI.hpp"
#include "View__Help.hpp"

LevelState::LevelState() :
	levelView(NULL),
	levelMap(NULL),
	paused(false),
  view__help(NULL),
  view__hiring(NULL)
{
	// Initialize views
	levelView = new LevelView();
	addView(levelView);

  view__btmUIBar = new View__BottomBar();
  addView(view__btmUIBar);

  openView_help();
	
	// Create map
	levelMap = new LevelMap(this, levelView);
	
	// Time
	realtimetime = 0.0;
	realtimetimer = new W::Timer();
	
//	MrKlangy::playBGM("level.mod");
	
	W::Messenger::subscribe(W::EventType::KeyUp, W::Callback(&LevelState::keyEvent, this));
  W::Messenger::subscribeToUIEvent("help_close_btn", W::EventType::ButtonClick, W::Callback(&LevelState::buttonEvent, this));
  W::Messenger::subscribeToUIEvent("open_hiring_view", W::EventType::ButtonClick, W::Callback(&LevelState::buttonEvent, this));
  W::Messenger::subscribeToUIEvent("close_hiring_view", W::EventType::ButtonClick, W::Callback(&LevelState::buttonEvent, this));
  W::Messenger::subscribeToUIEvent("hire_staff", W::EventType::ButtonClick, W::Callback(&LevelState::buttonEvent, this));
}
LevelState::~LevelState()
{
	removeView(levelView);
	delete levelView;
	delete levelMap;
	
//	MrKlangy::stopBGM();
}

W::EventPropagation::T LevelState::keyEvent(W::Event *ev) {
  // Quit
  if (ev->key == W::KeyCode::ESC) { W::popState(W::EmptyReturny); }
  else if (ev->key == W::KeyCode::_Q)  { W::popState(W::KillerReturny); }

  // Save
  else if (ev->key == W::KeyCode::_S) {
    saveLevel("a save game " + std::to_string(W::Rand::intUpTo(10000000)));
  }

  // Help view open/close
  else if (ev->key == W::KeyCode::_H)  {
    if (view__help) { closeView_help(); }
    else { openView_help(); }
  }

  // Pause/unpause
  else if (ev->key == W::KeyCode::_P) {
    if (!view__help) {
      if (paused) { unpause(); }
      else { pause(); }
    }
  }

  return W::EventPropagation::ShouldContinue;
}

W::EventPropagation::T LevelState::buttonEvent(W::Event *ev) {
  std::string *name = (std::string*) ev->_payload;

  if (*name == "help_close_btn") {
    closeView_help();
  }

  // Hiring view
  else if (*name == "open_hiring_view") {
    openView_hiring();
  }
  else if (*name == "close_hiring_view") {
    closeView_hiring();
  }
  else if (*name == "hire_staff") {
    levelMap->createUnit(true, "shopkeeper", {-1,-1});
  }

  return W::EventPropagation::ShouldContinue;
}

void LevelState::update() {
	// Time
	if (paused) {
		return;
	}
	
	int microseconds_elapsed = (int) realtimetimer->getMicroseconds();
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

void LevelState::openView_help() {
  if (view__help) {
    return;
  }
  pause();
  view__help = new View__Help();
  addView(view__help);
}
void LevelState::closeView_help() {
  if (!view__help) {
    return;
  }
  unpause();
  removeView(view__help);
  delete view__help;
  view__help = NULL;
}

void LevelState::openView_hiring() {
  if (view__hiring) {
    return;
  }
  view__hiring = new View__Hiring();
  addView(view__hiring);
}
void LevelState::closeView_hiring() {
  if (!view__hiring) {
    return;
  }
  removeView(view__hiring);
  delete view__hiring;
  view__hiring = NULL;
}
