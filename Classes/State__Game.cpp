#include "State__Game.hpp"
#include "View__Game.hpp"
#include "LevelMap.hpp"
#include "MrPaths.hpp"
#include "Audio.hpp"

#include "TLO.hpp"
#include "Building.hpp"
#include "Furnishing.hpp"
#include "Unit.hpp"
#include "Controller.hpp"
#include "W.h"
#include "Serializer.hpp"

#include "Views__UI.hpp"
#include "View__Help.hpp"

std::vector<std::string> music_files{
  "level/1942.xm",
  "level/cant-be-rubicon.xm",
  "level/dubieduw.xm",
  "level/eye-to-eye.xm",
  "level/happy-shopping.xm",
  "level/introtune.xm",
};

void play_random_music_track() {
  int nTracks = (int) music_files.size();
  size_t i = (size_t) W::Rand::intUpTo(nTracks);
  Audio::playMusic(music_files[i]);
}

State__Game::State__Game() :
  view__game(NULL),
  levelMap(NULL),
  paused(false),
  view__help(NULL),
  view__hiring(NULL),
  view__furnishingPurchasing(NULL),
  frame(0)
{
  // Create game view
  view__game = new View__Game();
  addView(view__game);

  // Create bottom bar view
  view__btmBar = new View__BottomBar();
  addView(view__btmBar);

  // Create game runner
  levelMap = new LevelMap(this, view__game, view__btmBar);

  openView_help();

  // Time
  time_elapsed_s = 0.0;
  timer = new W::Timer();

  W::Messenger::subscribe(W::EventType::KeyUp, W::Callback(&State__Game::keyEvent, this));

  // UI triggered events
  W::Messenger::subscribeToUIEvent("close_help_view", W::EventType::ButtonClick, W::Callback(&State__Game::buttonEvent, this));

  W::Messenger::subscribeToUIEvent("open_hiring_view", W::EventType::ButtonClick, W::Callback(&State__Game::buttonEvent, this));
  W::Messenger::subscribeToUIEvent("close_hiring_view", W::EventType::ButtonClick, W::Callback(&State__Game::buttonEvent, this));
  W::Messenger::subscribeToUIEvent("open_furnishing_purchasing_view", W::EventType::ButtonClick, W::Callback(&State__Game::buttonEvent, this));
  W::Messenger::subscribeToUIEvent("close_furnishing_purchasing_view", W::EventType::ButtonClick, W::Callback(&State__Game::buttonEvent, this));
}

State__Game::~State__Game()
{
  removeView(view__game);
  delete view__game;
  delete levelMap;
}

W::EventPropagation::T State__Game::keyEvent(W::Event ev) {
  // Quit
  if (ev.key == W::KeyCode::ESC) {
    Audio::stopMusic();
    W::popState(W::EmptyReturny);
  }
  else if (ev.key == W::KeyCode::_Q) {
    Audio::stopMusic();
    W::popState(W::KillerReturny);
  }

  // Save
  else if (ev.key == W::KeyCode::_S) {
    saveLevel("a save game " + std::to_string(W::Rand::intUpTo(10000000)));
  }

  // Help view open/close
  else if (ev.key == W::KeyCode::_H)  {
    if (view__help) { closeView_help(); }
    else { openView_help(); }
  }

  // Pause/unpause
  else if (ev.key == W::KeyCode::_P) {
    if (!view__help) {
      if (paused) { unpause(); }
      else { pause(); }
    }
  }

  return W::EventPropagation::ShouldContinue;
}

W::EventPropagation::T State__Game::buttonEvent(W::Event ev) {
  std::string name = ev.payload;

  if (name == "close_help_view") {
    closeView_help();
  }

  // Hiring view
  else if (name == "open_hiring_view") {
    openView_hiring();
  }
  else if (name == "close_hiring_view") {
    closeView_hiring();
  }

  // Furnishing view
  else if (name == "open_furnishing_purchasing_view") {
    openView_furnishingPurchasing();
  }
  else if (name == "close_furnishing_purchasing_view") {
    closeView_furnishingPurchasing();
  }

  return W::EventPropagation::ShouldContinue;
}

void State__Game::update() {
  if (frame%60 == 0) {
    update_music();
  }

  if (paused) {
    return;
  }

  // Time
  int frame_microseconds = (int) timer->getMicroseconds();
  if (frame_microseconds > 100000) {
    frame_microseconds = 100000;
  }

  time_elapsed_s += frame_microseconds / 1000000.0;
  timer->reset();

  // Update
  if (levelMap) {
    levelMap->update(frame_microseconds, time_elapsed_s);
  }

  frame += 1;
}
void State__Game::resume(W::Returny *ret) {
  if (ret->type == W::ReturnyType::Killer) {
    W::popState(W::KillerReturny);
  }
  else if (ret->type == W::ReturnyType::Payload) {
    if (ret->payload == "exit to menu") W::popState(W::EmptyReturny);
    else if (ret->payload == "replay")  W::popState(*ret);
  }
}

void State__Game::pause() {
  paused = true;
}

void State__Game::unpause() {
  timer->reset();
  paused = false;
}

bool State__Game::loadLevel(const std::string &levelName) {
  using std::string;

  W::log << "State__Game: loading level '" << levelName << "'..." << std::endl;

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
  // levelMap also calls view__game.setLevelSize, and creates the NavMap.
  if (!mapload_success) {
    W::log << "Error loading map" << std::endl;
    return false;
  }

  W::log << "...loaded." << std::endl;

  view__help->setTimeRemaining(levelMap->get_time_remaining_s());
  view__help->setMonetaryTarget(levelMap->get_level_financial_target());

  return true;
}

bool State__Game::saveLevel(const std::string &saveName) {
  using std::string;

  string s = levelMap->save();

  // Prepend title
  s = string("-- ") + saveName + string("\n\n") + s;

  // Save file to save games directory
  string saveDir = MrPaths::settingsPath + "Saved games/";
  string saveFileName = saveDir + saveName + ".lua";
  if (!W::isValidDir(MrPaths::settingsPath)) {
    if (!W::createDir(MrPaths::settingsPath)) {
      return false;
    }
  }
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

void State__Game::openView_help() {
  if (view__help) {
    return;
  }
  pause();
  view__help = new View__Help();
  addView(view__help);
  view__help->setTimeRemaining(levelMap->get_time_remaining_s());
  view__help->setMonetaryTarget(levelMap->get_level_financial_target());
}

void State__Game::closeView_help() {
  if (!view__help) {
    return;
  }
  unpause();
  removeView(view__help);
  delete view__help;
  view__help = NULL;
}

void State__Game::openView_hiring() {
  if (view__hiring) {
    return;
  }
  view__hiring = new View__Hiring();
  addView(view__hiring);
}

void State__Game::closeView_hiring() {
  if (!view__hiring) {
    return;
  }
  removeView(view__hiring);
  delete view__hiring;
  view__hiring = NULL;
}

void State__Game::openView_furnishingPurchasing() {
  if (view__furnishingPurchasing) {
    return;
  }
  view__furnishingPurchasing = new View__FurnishingPurchasing(Furnishing::get_furnishing_types());
  addView(view__furnishingPurchasing);
}

void State__Game::closeView_furnishingPurchasing() {
  if (!view__furnishingPurchasing) {
    return;
  }
  removeView(view__furnishingPurchasing);
  delete view__furnishingPurchasing;
  view__furnishingPurchasing = NULL;
}

void State__Game::update_music() {
  if (!Audio::musicIsPlaying()) {
    play_random_music_track();
  }
}
