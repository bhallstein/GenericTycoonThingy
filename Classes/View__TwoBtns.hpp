/*
 * Generic Tycoon Thingy
 *
 * ====================
 *  View__TwoBtns.hpp
 * ====================
 *
 * Copyright (C) 2012 - Ben Hallstein
 * All rights reserved
 *
 */

#ifndef View__TwoBtns_h
#define View__TwoBtns_h

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
    elements.push_back(new W::Button(btn1_action,
                                     new W::Positioner(W::Corner::TopLeft,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       0.5 - offset_from_center - btn_w, 0.5, btn_w, 0.05),
                                     this));
    elements.push_back(new W::Button(btn2_action,
                                     new W::Positioner(W::Corner::TopLeft,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       W::PosType::Proportional,
                                                       0.5 + offset_from_center, 0.5, btn_w, 0.05),
                                     this));

    landscape_elements.push_back(elements);

    // Add other elements
    bgrect = new W::Rectangle(this, {0,0}, {0,0}, {0,0,0,0.8});

    win_lose_msg = new W::RetroText(this, {0,0}, msg, W::Colour::White, W::TextAlign::Centre);
    txt_replay = new W::RetroText(this, {0,0}, btn1_label, W::Colour::White, W::TextAlign::Centre);
    txt_quit   = new W::RetroText(this, {0,0}, btn2_label, W::Colour::White, W::TextAlign::Centre);

    updatePosition(W::windowSize());
  }
  virtual ~View__TwoBtns()
  {
    delete win_lose_msg;
    delete txt_replay;
    delete txt_quit;
  }

  void updatePosition__uiview(W::v2i winsize) {
    win_lose_msg->setPos({float(rct.size.a/2), 40.});

    txt_replay->setPos(W::v2f(float((0.5 - offset_from_center - btn_w/2.)*rct.size.a), rct.size.b * 0.6));
    txt_quit  ->setPos(W::v2f(float((0.5 + offset_from_center + btn_w/2.)*rct.size.a), rct.size.b * 0.6));
  }

private:
  W::RetroText *win_lose_msg;
  W::RetroText *txt_replay;
  W::RetroText *txt_quit;

  std::string msg;
  std::string btn1_label;
  std::string btn1_action;
  std::string btn2_label;
  std::string btn2_action;

  const float btn_w = 0.1;
  const float offset_from_center = 0.05;
};

#endif
