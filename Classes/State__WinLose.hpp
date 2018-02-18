/*
 * Generic Tycoon Thingy
 *
 * =====================
 *  State__WinLose.hpp
 * =====================
 *
 * Copyright (C) 2012 - Ben Hallstein
 * All rights reserved
 *
 */
 
#ifndef State__WinLose_H
#define State__WinLose_H

#include "W.h"

class View__TwoBtns;


class State__WinLose : public W::GameState {
public:
  State__WinLose(bool victory);
	~State__WinLose();

  void resume(W::Returny *) { }
  void update() { }

  W::EventPropagation::T keyEvent(W::Event*);
  W::EventPropagation::T btnEvent(W::Event*);
	
protected:
  View__TwoBtns *view;

  void replay();
  void exit_to_menu();
  void exit_completely();

  bool victory;
};

#endif
