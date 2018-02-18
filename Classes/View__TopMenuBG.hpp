#ifndef View__TopMenuBG_h
#define View__TopMenuBG_h

#include "W.h"

class View__MenuBG : public W::View {
public:
  View__MenuBG();
  ~View__MenuBG();

  void updatePosition(const W::size &winsize);
  void customOpenGLDrawing();

private:
  W::DRect *bg;
  std::vector<W::Colour> color_cycle;
  std::vector<size_t> color_indices;
  std::vector<W::DRect*> rects;
  std::vector<size_t> bar_transition_heights;
  bool randomised;

  size_t frame;
  float alpha;

  void incrementColour(size_t i);
  int color_phase_increment;

  struct Mode {
    enum T {
      None,
      Random,
      StripesH,
      StripesRev,
      Fade,
      Pulse,

      TransWipe,
      TransCirc,
      TransBars,

      Last,
    };
  };
  Mode::T mode;
  void switchMode();
};


#endif
