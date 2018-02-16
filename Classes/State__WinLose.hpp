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
	State__WinLose(bool victory);
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

  bool victory;
};



class View__WinLose  : public W::UIView {
public:
  View__WinLose(bool victory) : W::UIView()
  {
    orientation_check = false;
    orientation = O_LANDSCAPE;

    // Add positioner & replay/exit btns
    landscape_positioning_limits.push_back(100);
    landscape_positioners.push_back(new W::Positioner(W::Corner::TopLeft,
                                                      W::PosType::Proportional,
                                                      W::PosType::Proportional,
                                                      W::PosType::Proportional,
                                                      W::PosType::Proportional,
                                                      0.2, 0.2, 0.6, 0.6,
                                                      false));
    auto elements = element_list();
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
    bgDRect = new W::DRect(this, {0,0}, {0,0}, {0,0,0,0.8});

    std::string msg = victory ? "You win!" : "You lose!";
    win_lose_msg = new W::DText(this, {0,0}, msg, W::Colour::White, W::TextAlign::Centre);

    txt_replay = new W::DText(this, {0,0}, "Replay", W::Colour::White, W::TextAlign::Centre);
    txt_quit   = new W::DText(this, {0,0}, "Quit", W::Colour::White, W::TextAlign::Centre);

    updatePosition(W::_controller.window->getSize());
  }
  virtual ~View__WinLose()
  {
    delete win_lose_msg;
  }

  void updatePosition_uiview(W::size winsize) {
    win_lose_msg->setPos({rct.sz.width/2, 40});

    txt_replay->setPos(W::position(int((0.48 - btn_w/2.)*rct.sz.width), rct.sz.height * 0.6));
    txt_quit  ->setPos(W::position(int((0.52 + btn_w/2.)*rct.sz.width), rct.sz.height * 0.6));
  }

private:
  W::DText *win_lose_msg;
  W::DText *txt_replay;
  W::DText *txt_quit;

  constexpr static const float btn_w = 0.1;
};

#endif
