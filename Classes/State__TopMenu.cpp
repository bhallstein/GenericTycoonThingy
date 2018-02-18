/*
 * Generic Tycoon Thingy
 *
 * =====================
 *  State__TopMenu.cpp
 * =====================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#include "State__TopMenu.hpp"
#include "State__Game.hpp"
#include "View__TopMenuBG.hpp"
#include "MrKlangy.hpp"
#include "MrPaths.hpp"

State__TopMenu::State__TopMenu() : W::GameState(W::GS_OPAQUE)
{
  view__menuBG = new View__MenuBG();
  addView(view__menuBG);

  view__menuUI = new View__MenuUI();
  addView(view__menuUI);

	// Key subscriptions
	W::Messenger::subscribe(W::EventType::KeyUp, W::Callback(&State__TopMenu::keyEvent, this));
	W::Messenger::subscribeToUIEvent("LevelStartBtn", W::EventType::ButtonClick, W::Callback(&State__TopMenu::uiEvent, this));
	
//	MrKlangy::playBGM("menu.xm");
}
State__TopMenu::~State__TopMenu()
{
  removeView(view__menuBG);
  delete view__menuBG;

  removeView(view__menuUI);
  delete view__menuUI;
//	MrKlangy::stopBGM();
}

void State__TopMenu::resume(W::Returny *ret) {
//	MrKlangy::playBGM("menu.xm");
	if (ret->type == W::ReturnyType::Killer) {
		W::popState(W::KillerReturny);
	}
	else if (ret->type == W::ReturnyType::Payload) {
		if (ret->payload == "replay") startLevel("qj-level");
	}
}
void State__TopMenu::update() {
	
}

W::EventPropagation::T State__TopMenu::keyEvent(W::Event *ev) {
	if (ev->key == W::KeyCode::_Q || ev->key == W::KeyCode::ESC)
		W::popState(W::EmptyReturny);
	return W::EventPropagation::ShouldContinue;
}
W::EventPropagation::T State__TopMenu::uiEvent(W::Event *ev) {
	startLevel("qj-level");
	return W::EventPropagation::ShouldContinue;
}

void State__TopMenu::startLevel(const std::string &levelName) {
	W::log << "Starting level: " << levelName << std::endl;
	try {
		state__game = new State__Game();
		if (state__game->loadLevel(levelName))
			W::pushState(state__game);
		else
			delete state__game;
	} catch (W::Exception &exc) {
		W::log << "Error: " << exc.what() << std::endl;
	}
}


View__MenuUI::View__MenuUI() :
	W::UIView(MrPaths::resourcesPath + "Data/UIViews/MenuUI.lua")
{
  bgDRect->setCol({0, 0, 0, 0});
	t1 = new W::DText(this, W::position(186,30), "Welcome to Generic Tycoon Thingy", W::Colour::Black);
	t2 = new W::DText(this, W::position(308,60), "Click to begin", W::Colour::White);
}
