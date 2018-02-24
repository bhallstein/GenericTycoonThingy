/*
 * Generic Tycoon Thingy
 *
 * ===============
 *  Audio.cpp
 * ===============
 *
 * Copyright (C) 2012 - Ben Hallstein
 * All rights reserved
 *
 */

#include "Audio.hpp"
#include "MrPaths.hpp"
#include <exception>
#include "W.h"

using std::string;


namespace Audio {

  string full_path(string audio_path) {
    return MrPaths::resourcesPath + string("Data/Audio/") + audio_path;
  }


  // Player (singleton - get with get_player()
  // -----------------------------------

  struct Player {
    Player() :
    sound_engine(NULL),
    background_music(NULL)
    {
      irrklang::ISoundDeviceList *deviceList = irrklang::createSoundDeviceList();
      int n_devices = deviceList->getDeviceCount() > 0;
      deviceList->drop();

      if (n_devices == 0) {
        throw new std::exception();
      }

      sound_engine = irrklang::createIrrKlangDevice();
    }

    irrklang::ISoundEngine *sound_engine;
    irrklang::ISound *background_music;

    // For when playing in multiple song mode
    bool playing_multi;
    std::vector<string> songs;
    string cur_song;
  };

  Player* get_player() {  // -> KlangState or null
    static bool inited = false;
    static Player *p = NULL;
    if (!inited) {
      try {
        p = new Player;
      }
      catch (std::exception exc) { }
      inited = true;
    }
    return p;
  }


  // Single song mode
  // -----------------------------------

  void playSound(string filename) {
    auto p = get_player();
    if (p) {
      string path = full_path(filename);
      p->sound_engine->play2D(path.c_str(), false);
    }
  }

  void playBGM(string filename, bool loop, bool stop_cur) {
    auto p = get_player();
    if (p) {
      if (stop_cur) {
        stopBGM();
        p->playing_multi = false;
      }

      string path = MrPaths::resourcesPath + string("Data/Audio/") + filename;
      p->background_music = p->sound_engine->play2D(path.c_str(), loop, false, true);
    }
  }

  void stopBGM() {
    auto p = get_player();
    if (p && p->background_music) {
      p->background_music->stop();
      p->background_music->drop();
      p->background_music = NULL;
    }
  }


  // Simple random playlist functionality
  // -----------------------------------

  void next() {
    auto p = get_player();
    if (p) {
      stopBGM();
      size_t i = size_t(W::Rand::intUpTo(int(p->songs.size())));

      p->cur_song = p->songs[i];
      p->playing_multi = true;
      playBGM(p->cur_song, false, false);
    }
  }

  void playBGM_multi(std::vector<string> filenames) {
    auto p = get_player();
    if (p) {
      stopBGM();

      p->songs = filenames;
      next();
    }
  }

  void updateBGM_multi() {
    auto p = get_player();
    if (p && p->playing_multi) {
      string path = full_path(p->cur_song);
      if (!p->sound_engine->isCurrentlyPlaying(path.c_str())) {
        next();
      }
    }
  }

}

