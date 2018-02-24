/*
 * Generic Tycoon Thingy
 *
 * ===============
 *  Audio.hpp
 * ===============
 *
 * Copyright (C) 2012 - Ben Hallstein
 * All rights reserved
 *
 */

#ifndef Audio_H
#define Audio_H

#include "irrKlang.h"
#include <vector>

namespace Audio {
  void playBGM(std::string filename, bool loop = true, bool stop_cur = true);
  void stopBGM();

  void playSound(std::string filename);

  // Random playlist
  void playBGM_multi(std::vector<std::string> filenames);
  void updateBGM_multi();
    // Call update~() regularly to play the next song
    // automatically when the previous one has ended
};

#endif

