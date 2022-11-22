#ifndef StateTopMenu_h
#define StateTopMenu_h

#include "W.h"

class State__Game;

class View__MenuBG;
class View__TwoBtns;


class State__TopMenu : public W::GameState {
public:
  State__TopMenu();
  ~State__TopMenu();

  void resume(W::Returny *);
  void update();

  W::EventPropagation::T keyEvent(W::Event);
  W::EventPropagation::T uiEvent(W::Event);

  void startLevel(const std::string &levelName);
  void startLevelOne();

protected:
  View__MenuBG *view__menuBG;
  View__TwoBtns *view__twoBtns;

  State__Game *state__game;

  size_t frame;

  static void update_music();

  void quit() {
    W::popState(W::KillerReturny);
  }
};

#endif
