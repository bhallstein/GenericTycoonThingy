/*
 * Generic Tycoon Thingy
 *
 * ================
 *  Views__UI.hpp
 * ================
 *
 * Copyright (C) 2012 - Ben Hallstein
 * All rights reserved
 *
 */

#ifndef Views__UI_h
#define Views__UI_h

#include "W.h"
#include "MrPaths.hpp"

class View__BottomBar : public W::UIView {
public:
  View__BottomBar()
    : W::UIView(MrPaths::resourcesPath + "Data/UIViews/BottomBar.lua")
  {
    // Also want to add the economy, & setter for its contents
    d_econ = new W::RetroText(this, {0,0}, "", W::Colour::White, W::TextAlign::Right);
    setEcon(0);
    d_econ->setPos({float(rct.size.a - 10), 10});
  }

  void setEcon(int money) {
    char econ[14];
    sprintf(econ, "%c%d", MR_CURRENCY, money);
    d_econ->setText(econ);
  }

  void updatePosition(W::v2i winsize) {
    W::UIView::updatePosition(winsize);
    d_econ->setPos({float(rct.size.a - 10), 10});
  }

  W::RetroText *d_econ;
};


class View__Hiring : public W::UIView {
public:
  View__Hiring() : W::UIView(MrPaths::resourcesPath + "Data/UIViews/Hiring.lua") { }
};


class View__FurnishingPurchasing : public W::UIView {
public:
  View__FurnishingPurchasing(std::vector<std::string> furnishingTypes) :
    W::UIView(MrPaths::resourcesPath + "Data/UIViews/FurnishingPurchasing.lua")
  {
    // Dynamically populate furnishing type buttons
    int i=0;
    for (auto t : furnishingTypes) {
      std::string s = std::string("buy_furnishing:") + t;
      W::Button *b = new W::Button(s.c_str(),
                                   W::Positioner(W::Corner::TopLeft,
                                                 W::PosType::Fixed,
                                                 W::PosType::Fixed,
                                                 W::PosType::Fixed,
                                                 W::PosType::Fixed,
                                                 7 + (20+10)*i, 30,
                                                 20, 20),
                                   this);
      landscape_elements[0].push_back(b);
      b->activate();
      i += 1;
    }

    updatePosition(W::windowSize());
  }
};


#endif
