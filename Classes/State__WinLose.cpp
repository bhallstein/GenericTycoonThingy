#include "State__WinLose.hpp"
#include "State__Game.hpp"
#include "View__TwoBtns.hpp"
#include "Audio.hpp"
#include "MrPaths.hpp"

State__WinLose::State__WinLose(bool _victory) :
  W::GameState(W::GS_TRANSLUCENT),
  victory(_victory)
{
  view = new View__TwoBtns(victory ? "You win!" : "You lose!",
                           "Replay",
                           "winlose__replay",
                           "Menu",
                           "winlose__exit_to_menu");
  addView(view);

  W::Messenger::subscribe(W::EventType::KeyDown, W::Callback(&State__WinLose::keyEvent, this));
  W::Messenger::subscribeToUIEvent("winlose__replay", W::EventType::ButtonClick, W::Callback(&State__WinLose::btnEvent, this));
  W::Messenger::subscribeToUIEvent("winlose__exit_to_menu", W::EventType::ButtonClick, W::Callback(&State__WinLose::btnEvent, this));

  Audio::stopMusic();
  winlose_sound = Audio::playSound(victory ? "fx/winlose-win.mp3" : "fx/winlose-lose.mp3");
}

State__WinLose::~State__WinLose()
{
  if (winlose_sound) {
    Audio::stopSound(winlose_sound);
  }
  removeView(view);
  delete view;
}

W::EventPropagation::T State__WinLose::keyEvent(W::Event ev) {
  if (ev.key == W::KeyCode::_R) {
    replay();
  }
  else if (ev.key == W::KeyCode::ESC) {
    exit_to_menu();
  }
  else if (ev.key == W::KeyCode::_Q) {
    exit_completely();
  }

  return W::EventPropagation::ShouldStop;
}

W::EventPropagation::T State__WinLose::btnEvent(W::Event ev) {
  std::string s = ev.payload;

  if (s == "winlose__replay") {
    replay();
  }
  else if (s == "winlose__exit_to_menu") {
    exit_to_menu();
  }

  return W::EventPropagation::ShouldStop;
}

void State__WinLose::replay() {
  W::Returny r(W::ReturnyType::Payload);
  r.payload = "replay";
  popState(r);
}

void State__WinLose::exit_to_menu() {
  W::Returny r(W::ReturnyType::Payload);
  r.payload = "exit to menu";
  popState(r);
}

void State__WinLose::exit_completely() {
  popState(W::KillerReturny);
}
