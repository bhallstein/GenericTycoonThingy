/*
 * Placeable.hpp - sort of an abstract placement manager for TLOs
 *
 */

#ifndef Placeable_h
#define Placeable_h

#include <iostream>
#include <map>

#include "W.h"

class PlaceableManager;
class View__Game;

class Placeable {
public:
  Placeable(PlaceableManager *, View__Game *);
  ~Placeable();
  bool activate();
  void deactivate();
  W::EventPropagation::T mouseEvent(W::Event);
  PlaceableManager *mngr;
  W::v2f pos;
  View__Game *view__game;
};

#endif
