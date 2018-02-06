#ifndef Views__UI_hpp
#define Views__UI_hpp

#include "W.h"

class View__BottomBar : public W::UIView {
public:
  View__BottomBar() : W::UIView(MrPaths::resourcesPath + "Data/UIViews/BottomBar.lua") { }
  // Also want to add the economy, & setter for its contents
  //  char econ[14];
  //  sprintf(econ, "%c%d", MR_CURRENCY, *economy);
  //  drawText(790, 10, W::Colour::White, econ, true);
};


class View__FurnishingPurchasing : public W::UIView {
public:
  View__FurnishingPurchasing() : W::UIView("UIViews/FurnishingPurchasing.lua") { }
};


class View__Hiring : public W::UIView {
public:
  View__Hiring() : W::UIView(MrPaths::resourcesPath + "Data/UIViews/Hiring.lua") { }
};

#endif
