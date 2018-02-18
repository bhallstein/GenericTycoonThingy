/*
 * Generic Tycoon Thingy
 *
 * =====================
 *  State__TopMenu.hpp
 * =====================
 *
 * Copyright (C) 2012 - Ben Hallstein
 * All rights reserved
 *
 */

#ifndef State__TopMenu_H
#define State__TopMenu_H

#include "W.h"

class State__Game;

class View__MenuBG;
class View__TwoBtns;


class State__TopMenu : public W::GameState {
public:
  State__TopMenu();
  ~State__TopMenu();

  // Methods
  void resume(W::Returny *);
  void update();

  W::EventPropagation::T keyEvent(W::Event *);
  W::EventPropagation::T uiEvent(W::Event *);

  void startLevel(const std::string &levelName);
  void startLevelOne();

protected:
  View__MenuBG *view__menuBG;
  View__TwoBtns *view__twoBtns;

  State__Game *state__game;

  void quit() {
    W::popState(W::KillerReturny);
  }
};


#endif

