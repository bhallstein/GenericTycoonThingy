#ifndef ViewTwoBtns_h
#define ViewTwoBtns_h

#include "W.h"

class View__TwoBtns  : public W::UIView {
public:
  View__TwoBtns(std::string _msg,
                std::string _btn1_label,
                std::string _btn1_action,
                std::string _btn2_label,
                std::string _btn2_action) :
  W::UIView(),
  msg(_msg),
  btn1_label(_btn1_label),
  btn1_action(_btn1_action),
  btn2_label(_btn2_label),
  btn2_action(_btn2_action)
  {
    orientation_check = false;
    orientation = O_LANDSCAPE;

    // Add other elements
    bgrect = new W::Rectangle(this, {0,0}, {0,0}, {0,0,0,0.8});
    txt_title = new W::RetroText(this, {0,0}, msg, W::Colour::White, W::TextAlign::Centre);

    // Add positioner & replay/exit btns
    landscape_positioning_limits.push_back(100);
    landscape_positioners.push_back(W::Positioner(W::Corner::TopLeft,
                                                  W::PosType::Proportional,
                                                  W::PosType::Proportional,
                                                  W::PosType::Proportional,
                                                  W::PosType::Proportional,
                                                  0.2, 0.2, 0.6, 0.6,
                                                  false));
    auto elements = element_list();
    elements.push_back(new W::TextButton(btn1_label,
                                         btn1_action,
                                         W::Positioner(W::Corner::TopLeft,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       0.5 - offset_from_center - btn_w, 0.5, btn_w, 0.05),
                                         this));
    elements.push_back(new W::TextButton(btn2_label,
                                         btn2_action,
                                         W::Positioner(W::Corner::TopLeft,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       0.5 + offset_from_center, 0.5, btn_w, 0.05),
                                         this));

    landscape_elements.push_back(elements);

    updatePosition(W::windowSize());
  }
  virtual ~View__TwoBtns()
  {
    delete txt_title;
  }

  void updatePosition__uiview(W::v2i winsize) {
    txt_title->setPos({float(rct.size.a/2), 40.});
  }

private:
  W::RetroText *txt_title;

  std::string msg;
  std::string btn1_label;
  std::string btn1_action;
  std::string btn2_label;
  std::string btn2_action;

  const float btn_w = 0.1;
  const float offset_from_center = 0.05;
};

#endif
