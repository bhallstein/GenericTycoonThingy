#ifndef Views__UI_hpp
#define Views__UI_hpp

#include "W.h"

class View__BottomBar : public W::UIView {
public:
  View__BottomBar()
    : W::UIView(MrPaths::resourcesPath + "Data/UIViews/BottomBar.lua")
  {
    // Also want to add the economy, & setter for its contents
    d_econ = new W::DText(this, {0,0}, "", W::Colour::White, W::TextAlign::Right);
    setEcon(7);
    d_econ->setPos({rct.sz.width - 10, 10});
  }

  void setEcon(int money) {
    char econ[14];
    sprintf(econ, "%c%d", MR_CURRENCY, money);
    d_econ->setTxt(econ);
  }

  void updatePosition(const W::size &winsize) {
    W::UIView::updatePosition(winsize);
    d_econ->setPos({rct.sz.width - 10, 10});
  }

  W::DText *d_econ;
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
                                   new W::Positioner(W::Corner::TopLeft,
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

    updatePosition(W::_controller.window->getSize());
  }
};


#endif
