#include "View__TopMenuBG.hpp"

#include <limits>
#include <cmath>

View__MenuBG::View__MenuBG() :
W::View(W::Positioner::WholeAreaPositioner)
{
  bg = new W::Rectangle(this, {0,0}, {0,0}, W::Colour::Black);

  color_cycle.resize(60);
  for (int i=0; i < 60; i++) {
    W::Colour &c = color_cycle[i];
    c.a = 0.1;
    if (i < 10) {  // red -> yellow
      c.r = 1;
      c.g = i/10.;
      c.b = 0;
    }
    else if (i < 20) {  // -> green
      c.r = (20-i)/10.;
      c.g = 1;
      c.b = 0;
    }
    else if ( i < 30) {  // -> cyan
      c.r = 0;
      c.g = 1;
      c.b = (i-20)/10.;
    }
    else if (i < 40) {  // -> blue
      c.r = 0;
      c.g = (40-i)/10.;
      c.b = 1;
    }
    else if (i < 50) {  // -> magenta
      c.r = (i-40)/10.;
      c.g = 0;
      c.b = 1;
    }
    else {  // -> red
      c.r = 1;
      c.g = 0;
      c.b = (60-i)/10.;
    }
  }

  color_indices.resize(40 * 30);
  for (int i=0; i < 40 * 30; ++i) {
    rects.push_back(new W::Rectangle(this, {0,0}, {0,0}, color_cycle[0]));
  }

  frame = 0;
  alpha = 0;
  mode = Mode::None;
  randomised = false;

  updatePosition(W::windowSize());
}
View__MenuBG::~View__MenuBG() {
  for (auto r : rects) {
    delete r;
  }
  delete bg;
}

void View__MenuBG::updatePosition(W::v2i winsize) {
  float rw = winsize.a / 40.;
  float rh = winsize.b / 30.;

  for (int i=0; i < 40; ++i) {
    for (int j=0; j < 30; ++j) {
      int n = j*40 + i;
      W::Rectangle *r = rects[n];

      r->setPos({rw*i, rh*j});
      r->setSz({rw, rh});
    }
  }

  bg->setSz(rct.size);
}

void View__MenuBG::customOpenGLDrawing() {
  if (mode == Mode::None || ++frame > 100) {
    switchMode();
  }

  if (alpha < 0.8) {
    alpha += 0.08;
  }

  // Transitions
  if (mode == Mode::TransWipe) {
    int x = (int) (40 - frame - 1);
    if (x >= 0) {
      for (int y=0; y < 30; y++) {
        color_indices[40*y + x] = W::Rand::intUpTo((int) color_cycle.size());
      }
    }
  }
  else if (mode == Mode::TransCirc) {
    int outer_radius = (int) (40 - frame);
    int inner_radius = outer_radius - 2;
    for (int i=0; i < 40*30; i++) {
      int x = i%40-20;
      int y = i/40-15;
      int dist = sqrt((double)(x*x + y*y));
      if (dist < outer_radius && dist >= inner_radius) {
        color_indices[i] = W::Rand::intUpTo((int) color_cycle.size());
      }
    }
  }
  else if (mode == Mode::TransBars) {
    for (int x=0; x < 40; x++) {
      int y = (int) (frame - bar_transition_heights[x] - 1);
      if (y >= 0 && y < 30) {
        color_indices[40*y + x] = W::Rand::intUpTo((int) color_cycle.size());
      }
    }
  }

  for (int i=0; i < 40 * 30; ++i) {
    incrementColour(i);
  }

  for (int i=0; i < 40 * 30; ++i) {
    W::Colour c = color_cycle[color_indices[i]];
    c.a = alpha;
    rects[i]->setCol(c);
  }
}

void View__MenuBG::incrementColour(size_t i) {
  size_t &ind = color_indices[i];
  ind += color_phase_increment;
  if (color_phase_increment > 0 && ind >= color_cycle.size()) {
    ind -= color_cycle.size();
  }
  else if (color_phase_increment < 0 && ind >= std::numeric_limits<size_t>::max()-5) {
    ind += color_cycle.size();
  }
}

void View__MenuBG::switchMode() {
  frame = 0;
  color_phase_increment = 1;

  // If a transition mode, put back to first mode
  if (mode >= Mode::TransWipe) {
    mode = Mode::Random;
    randomised = true;
  }
  else {
    Mode::T new_mode = Mode::T(int(mode) + 1);
    if (new_mode == Mode::TransWipe) {
      new_mode = Mode::T(int(new_mode) + 0);// W::Rand::intUpTo(3));
    }
    mode = new_mode;
  }

  // Set up mode state
  if (mode == Mode::Random && !randomised) {
    for (auto &i : color_indices) {
      i = W::Rand::intUpTo((int) color_cycle.size());
    }
  }

  else if (mode == Mode::StripesH || mode == Mode::StripesRev) {
    for (size_t x=0; x < 40; ++x) {
      for (size_t y=0; y < 30; ++y) {
        size_t i = 40*y + x;
        color_indices[i] = (x + 30)%color_cycle.size();
      }
    }
    color_phase_increment = mode == Mode::StripesH ? 2 : -2;
  }

  else if (mode == Mode::Fade) {
    int ind = W::Rand::intUpTo((int) color_cycle.size());
    for (auto &i : color_indices) {
      i = ind;
    }
  }

  else if (mode == Mode::Pulse) {
    for (int i=0; i < 40*30; i++) {
      int x = i%40 - 20;
      int y = i/40 - 15;
      int _h = sqrt((double) (x*x + y*y));
      color_indices[i] = _h%color_cycle.size();
    }
  }

  else if (mode == Mode::TransBars) {
    bar_transition_heights.resize(40);
    for (auto &h : bar_transition_heights) {
      h = W::Rand::intUpTo(15);
    }
  }
}
