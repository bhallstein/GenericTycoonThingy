/*
 * Generic Tycoon Thingy
 *
 * =====================
 *  State__TopMenu.cpp
 * =====================
 *
 * Copyright (C) 2012 - Ben Hallstein
 * All rights reserved
 *
 */

#include "State__TopMenu.hpp"
#include "State__Game.hpp"
#include "View__TopMenuBG.hpp"
#include "View__TwoBtns.hpp"
#include "Audio.hpp"
#include "MrPaths.hpp"

State__TopMenu::State__TopMenu() : W::GameState(W::GS_OPAQUE)
{
  view__menuBG = new View__MenuBG();
  addView(view__menuBG);

  view__twoBtns = new View__TwoBtns("Generic Tycoon Thingy",
                                    "Play",
                                    "start-game",
                                    "Quit",
                                    "quit");
  addView(view__twoBtns);

  // Key subscriptions
  W::Messenger::subscribe(W::EventType::KeyUp, W::Callback(&State__TopMenu::keyEvent, this));
  W::Messenger::subscribeToUIEvent("start-game", W::EventType::ButtonClick, W::Callback(&State__TopMenu::uiEvent, this));
  W::Messenger::subscribeToUIEvent("quit", W::EventType::ButtonClick, W::Callback(&State__TopMenu::uiEvent, this));

  play_music();
}
State__TopMenu::~State__TopMenu()
{
  removeView(view__menuBG);
  delete view__menuBG;

  removeView(view__twoBtns);
  delete view__twoBtns;

  Audio::stopBGM();
}

void State__TopMenu::resume(W::Returny *ret) {
  if (ret->type == W::ReturnyType::Killer) {
    quit();
  }
  else if (ret->type == W::ReturnyType::Payload) {
    if (ret->payload == "replay") {
      startLevel("qj-level");
    }
  }
  else {
    play_music();
  }
}

W::EventPropagation::T State__TopMenu::keyEvent(W::Event *ev) {
  if (ev->key == W::KeyCode::_Q || ev->key == W::KeyCode::ESC) {
    W::popState(W::KillerReturny);
  }
  return W::EventPropagation::ShouldContinue;
}

W::EventPropagation::T State__TopMenu::uiEvent(W::Event *ev) {
  std::string *payload = (std::string*) ev->_payload;

  if (*payload == "quit") {
    quit();
  }
  else if (*payload == "start-game") {
    startLevel("qj-level");
  }

  return W::EventPropagation::ShouldContinue;
}

void State__TopMenu::startLevel(const std::string &levelName) {
	W::log << "Starting level: " << levelName << std::endl;
	try {
		state__game = new State__Game();
    if (state__game->loadLevel(levelName)) {
      Audio::stopBGM();
			W::pushState(state__game);
    }
    else {
			delete state__game;
    }
	} catch (W::Exception &exc) {
		W::log << "Error: " << exc.what() << std::endl;
	}
}

void State__TopMenu::play_music() {
  Audio::playBGM("menu.xm");
}
