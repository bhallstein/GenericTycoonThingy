#include "View__Game.hpp"

View__Game::View__Game() :
  View(W::Positioner(W::Corner::TopLeft,
                     W::PosType::Fixed,
                     W::PosType::Fixed,
                     W::PosType::Proportional,
                     W::PosType::Proportional,
                     0, 0, 1, 1)),
  gridsize(20),
  level_width(-1), level_height(-1),
  remaining_time_txt(NULL)
{
  bgrect = new W::Rectangle(this, rct.position, rct.size, W::Colour::White);

  // Screenedge scrolling
  W::Callback scrCb(&View__Game::scrollEvent, this);
  W::Messenger::subscribe(W::EventType::ScreenEdgeLeft,   scrCb);
  W::Messenger::subscribe(W::EventType::ScreenEdgeRight,  scrCb);
  W::Messenger::subscribe(W::EventType::ScreenEdgeTop,    scrCb);
  W::Messenger::subscribe(W::EventType::ScreenEdgeBottom, scrCb);
}
View__Game::~View__Game()
{
  W::Messenger::unsubscribe(W::EventType::ScreenEdgeLeft,   this);
  W::Messenger::unsubscribe(W::EventType::ScreenEdgeRight,  this);
  W::Messenger::unsubscribe(W::EventType::ScreenEdgeTop,    this);
  W::Messenger::unsubscribe(W::EventType::ScreenEdgeBottom, this);
}

void View__Game::updatePosition(W::v2i winsize) {
  bgrect->setSz(rct.size);
}

void View__Game::convertEventCoords(W::Event &ev) {
  ev.pos = convertPixelToGridCoords(ev.pos);
}
W::EventPropagation::T View__Game::scrollEvent(W::Event ev) {
  using namespace W::EventType;
  int scrollDist = 10;

  if (ev.type == ScreenEdgeLeft)        { scroll(-scrollDist, 0); }
  else if (ev.type == ScreenEdgeRight)  { scroll(scrollDist, 0); }
  if (ev.type == ScreenEdgeTop)         { scroll(0, -scrollDist); }
  else if (ev.type == ScreenEdgeBottom) { scroll(0, scrollDist); }

  return W::EventPropagation::ShouldContinue;
}
void View__Game::scroll(int x, int y) {
  _offset.a -= x;
  _offset.b -= y;
    // The offset is added to DrawnObjs’ positions to achieve scrolling.
    // When the offset is zero, we view the top left of the map; it can’t be greater than zero
  if (_offset.a > 0) _offset.a = 0;
  if (_offset.b > 0) _offset.b = 0;
    // The max (i.e. maximally negative) offset is the difference between the pixel size of the map
    // and the view’s actual on-screen size
  int max_offset_x = rct.size.a - level_width * gridsize;
  int max_offset_y = rct.size.b - level_height * gridsize;
  if (_offset.a < max_offset_x) _offset.a = max_offset_x;
  if (_offset.b < max_offset_y) _offset.b = max_offset_y;

  center_if_necessary();
  bgrect->setPos(W::v2i(-_offset.a, -_offset.b));
}

void View__Game::center_if_necessary() {
  if (rct.size.a >= gridsize * level_width) {
    _offset.a = (rct.size.a - gridsize * level_width) / 2;
  }
  if (rct.size.b >= gridsize * level_height) {
    _offset.b = (rct.size.b - gridsize * level_height) / 2;
  }

  bgrect->setPos(W::v2i(-_offset.a, -_offset.b));
}

void View__Game::setRemainingTime(float seconds) {
  char s[100];
  int time_minutes = seconds / 60;
  int time_seconds = int(seconds)%60;
  sprintf(s, "%02d:%02d", time_minutes, time_seconds);

  if (!remaining_time_txt) {
    remaining_time_txt = new W::RetroText(this,
                                          {10,10},
                                          s,
                                          W::Colour::TransparentBlack,
                                          W::TextAlign::Left);
  }
  else {
    remaining_time_txt->setText(s);
  }
}

W::v2f View__Game::convertGridToPixelCoords(W::v2f p) {
  return p * gridsize;
}
W::v2f View__Game::convertPixelToGridCoords(W::v2f p) {
  return p / gridsize;
}
