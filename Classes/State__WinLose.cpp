/*
 * Generic Tycoon Thingy
 *
 * =====================
 *  State__WinLose.cpp
 * =====================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#include "State__WinLose.hpp"
#include "State__Game.hpp"
#include "MrKlangy.hpp"
#include "MrPaths.hpp"

State__WinLose::State__WinLose() :
  W::GameState(W::GS_TRANSLUCENT)
{
  view = new View__WinLose();
	addView(view);

  bg = new W::DRect(view,
                    {0,0},
                    {100,100},
                    W::Colour::TransparentBlack);
	

  W::Messenger::subscribe(W::EventType::KeyUp, W::Callback(&State__WinLose::keyEvent, this));
  W::Messenger::subscribeToUIEvent("winlose__replay", W::EventType::ButtonClick, W::Callback(&State__WinLose::btnEvent, this));
  W::Messenger::subscribeToUIEvent("winlose__exit_to_menu", W::EventType::ButtonClick, W::Callback(&State__WinLose::btnEvent, this));
}
State__WinLose::~State__WinLose()
{
  delete bg;
	removeView(view);
  delete view;
}

W::EventPropagation::T State__WinLose::keyEvent(W::Event *ev) {
  if (ev->key == W::KeyCode::_R) {
    replay();
  }
  else if (ev->key == W::KeyCode::ESC) {
    exit_to_menu();
  }
  else if (ev->key == W::KeyCode::_Q) {
    exit_completely();
  }

  return W::EventPropagation::ShouldStop;
}
W::EventPropagation::T State__WinLose::btnEvent(W::Event *ev) {
  std::string *s = (std::string*) ev->_payload;

  if (*s == "winlose__replay") {
    replay();
  }
  else if (*s == "winlose__exit_to_menu") {
    exit_to_menu();
  }

  return W::EventPropagation::ShouldStop;
}

void State__WinLose::replay() {
  W::Returny r(W::ReturnyType::Payload);
  r.payload = "replay";
  popState(r);
}
void State__WinLose::exit_to_menu() {
  W::Returny r(W::ReturnyType::Payload);
  r.payload = "exit to menu";
  popState(r);
}
void State__WinLose::exit_completely() {
  popState(W::KillerReturny);
}

//W::EventPropagation::T State__WinLose::keyEvent(W::Event *ev) {
//  if (ev->key == W::KeyCode::_Q || ev->key == W::KeyCode::ESC)
//    W::popState(W::EmptyReturny);
//  return W::EventPropagation::ShouldContinue;
//}
//W::EventPropagation::T State__WinLose::uiEvent(W::Event *ev) {
//  startLevel("qj-level");
//  return W::EventPropagation::ShouldContinue;
//}

