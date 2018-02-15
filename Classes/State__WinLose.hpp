/*
 * Generic Tycoon Thingy
 *
 * =====================
 *  State__WinLose.hpp
 * =====================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

/*
 * State__WinLose.hpp - a primitive menu state
 * 
 * Instantiates a ClickToBeginView, which is a UIView containing a single button
 * and a MenuBackgroundView, which has flashy colours.
 *
 */ 
 
#ifndef State__WinLose_H
#define State__WinLose_H

#include "W.h"

class View__WinLose;


class State__WinLose : public W::GameState {
public:
	State__WinLose();
	~State__WinLose();

  void resume(W::Returny *) { }
  void update() { }

  W::EventPropagation::T keyEvent(W::Event*);
  W::EventPropagation::T btnEvent(W::Event*);
	
//  W::EventPropagation::T keyEvent(W::Event *);
//  W::EventPropagation::T uiEvent(W::Event *);
	
protected:
  View__WinLose *view;

  void replay();
  void exit_to_menu();
  void exit_completely();

  W::DRect *bg;

};



class View__WinLose  : public W::UIView {
public:
  View__WinLose() : W::UIView()
  {
    orientation_check = false;
    orientation = O_LANDSCAPE;

    // Add positioner & close button
    landscape_positioning_limits.push_back(100);
    landscape_positioners.push_back(new W::Positioner(W::Corner::TopLeft,
                                                      W::PosType::Proportional,
                                                      W::PosType::Proportional,
                                                      W::PosType::Proportional,
                                                      W::PosType::Proportional,
                                                      0.2, 0.2, 0.6, 0.6,
                                                      false));
    auto elements = element_list();
    float btn_w = 0.1;
    elements.push_back(new W::Button("winlose__replay",
                                     new W::Positioner(W::Corner::TopLeft,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       0.48 - btn_w, 0.5, btn_w, 0.05),
                                     this));
    elements.push_back(new W::Button("winlose__exit_to_menu",
                                     new W::Positioner(W::Corner::TopLeft,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       0.52, 0.5, btn_w, 0.05),
                                     this));

    landscape_elements.push_back(elements);

    // Add other elements
    bgDRect = new W::DRect(this, {0,0}, rct.sz, {0,0,0,0.8});

    updatePosition(W::_controller.window->getSize());
  }
  virtual ~View__WinLose()
  {

  }

private:

};

#endif
