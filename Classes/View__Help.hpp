/*
 * Generic Tycoon Thingy
 *
 * =================
 *  View__Help.hpp
 * =================
 *
 * Copyright (C) 2012 - Ben Hallstein
 * All rights reserved
 *
 */

#ifndef View__Help_h
#define View__Help_h

#include "W.h"

class View__Help  : public W::UIView {
public:
  View__Help() : W::UIView()
  {
    orientation_check = false;
    orientation = O_LANDSCAPE;

    // Add positioner & close button
    landscape_positioning_limits.push_back(100);
    landscape_positioners.push_back(W::Positioner(W::Corner::TopLeft,
                                                  W::PosType::Fixed,
                                                  W::PosType::Fixed,
                                                  W::PosType::Fixed,
                                                  W::PosType::Fixed,
                                                  10, 10, 500, 340,
                                                  true));
    auto elements = element_list();
    elements.push_back(new W::Button("close_help_view",
                                     W::Positioner(W::Corner::TopLeft,
                                                   W::PosType::Fixed,
                                                   W::PosType::Fixed,
                                                   W::PosType::Fixed,
                                                   W::PosType::Fixed,
                                                   10, 10, 10, 10),
                                     this));
    landscape_elements.push_back(elements);

    // Add other elements
    bgrect = new W::Rectangle(this, {0,0}, rct.size, {0,0,0,0.8});

    t1 = new W::RetroText(this, {220,10}, "Help", W::Colour::White);
    t2 = new W::RetroText(this, {14,54}, "place furniture and staff in your", W::Colour::White);
    t3 = new W::RetroText(this, {14,74}, "buildings. customers will come and", W::Colour::White);
    t4 = new W::RetroText(this, {14,94}, "give you money.", W::Colour::White);
    t5 = new W::RetroText(this, {14,144}, "[time left]", W::Colour::White);
    t6 = new W::RetroText(this, {14,184}, "Get cracking! GO!", W::Colour::White);
    r1 = new W::Rectangle(this, {10,234}, {480,1}, {1,1,1,0.4});
    t7 = new W::RetroText(this, {14,264}, "Esc: quit to main menu", W::Colour::White);
    t8 = new W::RetroText(this, {14,284}, "Q: quit completely", W::Colour::White);
    t9 = new W::RetroText(this, {14,304}, "H: this help screen", W::Colour::White);

    updatePosition(W::windowSize());
  }
  virtual ~View__Help()
  {
    delete t1;
    delete t2;
    delete t3;
    delete t4;
    delete t5;
    delete t6;
    delete t7;
    delete t8;
    delete t9;
    delete r1;
  }
  void setTimeRemaining(int t) {
    timeRemaining = t;
    setTimeRemainingText();
  }
  void setMonetaryTarget(int m) {
    monetaryTarget = m;
    setTimeRemainingText();
  }

private:
  int timeRemaining, monetaryTarget;

  W::RetroText *t1, *t2, *t3, *t4, *t5, *t6, *t7, *t8, *t9;
  W::Rectangle *r1;

  void setTimeRemainingText() {
    if (t5) delete t5;
    char s[100];
    int time_minutes = timeRemaining/60;
    int time_seconds = timeRemaining%60;
    sprintf(s, "you've got %02d:%02d left to earn %c%d", time_minutes, time_seconds, MR_CURRENCY, monetaryTarget);
    t5 = new W::RetroText(this, W::v2f(14,144), s, W::Colour::White);
  }

//  void updatePosition(const W::size &winsize) {
//    bg->setSz(rct.sz);
//    W::size pos = (winsize - rct.sz) / 2;
//    rct.pos = {pos.width, pos.height};
//  }
};

#endif
