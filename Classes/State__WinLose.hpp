#ifndef StateWinLose_h
#define StateWinLose_h

#include "W.h"

class View__TwoBtns;

class State__WinLose : public W::GameState {
public:
  State__WinLose(bool victory);
  ~State__WinLose();

  void resume(W::Returny *) { }
  void update() { }

  W::EventPropagation::T keyEvent(W::Event);
  W::EventPropagation::T btnEvent(W::Event);

protected:
  View__TwoBtns *view;

  void replay();
  void exit_to_menu();
  void exit_completely();

  void *winlose_sound;

  bool victory;
};

#endif
