#ifndef Views__UI_hpp
#define Views__UI_hpp

#include "W.h"

class View__BottomUIBar : public W::UIView {
public:
  View__BottomUIBar() : W::UIView(MrPaths::resourcesPath + "Data/UIViews/BottomUIBar.lua") { }
  // Also want to add the economy, & setter for its contents
  //  char econ[14];
  //  sprintf(econ, "%c%d", MR_CURRENCY, *economy);
  //  drawText(790, 10, W::Colour::White, econ, true);
};


class View__FurnishingPurchasingUIView : public W::UIView {
public:
  View__FurnishingPurchasingUIView() : W::UIView("UIViews/FurnishingPurchasingUIView.lua") { }
};


class View__HiringUIView : public W::UIView {
public:
  View__HiringUIView() : W::UIView("UIViews/HiringUIView.lua") { }
};

#endif
