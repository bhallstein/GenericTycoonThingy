#ifndef Audio_H
#define Audio_H

#include <vector>
#include <string>

namespace Audio {
  void* playSound(std::string filename); // Any supported sound type
  void stopSound(void* sound);           // Stops (and frees) a sound

  void playMusic(std::string filename);  // .xm files only
  bool musicIsPlaying();
  void stopMusic();
};

#endif
