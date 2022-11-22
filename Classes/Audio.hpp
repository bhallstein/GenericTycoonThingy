#ifndef Audio_H
#define Audio_H

#include <vector>
#include <string>

namespace Audio {
  void playSound(std::string filename);
  void playMusic(std::string filename);
  bool musicIsPlaying();
  void stopMusic();
};

#endif

