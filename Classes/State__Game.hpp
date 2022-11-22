#ifndef StateGame_h
#define StateGame_h

#include "W.h"

class LevelMap;
class Building;

/* View-related forward class declarations */
class View__Game;
class View__BottomBar;
class View__Help;
class View__Hiring;
class View__FurnishingPurchasing;


class State__Game : public W::GameState {
public:
  State__Game();
  ~State__Game();

  W::EventPropagation::T keyEvent(W::Event);
  W::EventPropagation::T buttonEvent(W::Event);

  void update();
  void resume(W::Returny *);

  void pause();
  void unpause();

  bool loadLevel(const std::string &levelName);
  bool saveLevel(const std::string &saveName);

  void openView_help();
  void closeView_help();

  void openView_hiring();
  void closeView_hiring();

  void openView_furnishingPurchasing();
  void closeView_furnishingPurchasing();

private:
  // Views
  View__Game *view__game;
  View__BottomBar *view__btmBar;
  View__Help      *view__help;
  View__Hiring    *view__hiring;
  View__FurnishingPurchasing *view__furnishingPurchasing;

  // Map
  LevelMap *levelMap;

  // Time
  bool paused;
  W::Timer *timer;
  float time_elapsed_s;

  size_t frame;

  static void update_music();
};

#endif
