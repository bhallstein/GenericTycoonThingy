#include "Audio.hpp"
#include "MrPaths.hpp"
#include <exception>
#include "W.h"
#define MINIAUDIO_IMPLEMENTATION
#define JAR_XM_IMPLEMENTATION
#define XMDS_IMPLEMENTATION
#include "xm-data-source.h"

namespace Audio {
  std::string full_path(std::string audio_path) {
    return MrPaths::resourcesPath + std::string("Data/Audio/") + audio_path;
  }
}


// Engine init
// ------------------------------

ma_engine engine;
bool have_engine = false;

void engine_init() {
  if (have_engine) {
    return;
  }

  W::log << "EngineInit()" << "\n";
  ma_result engine_init = ma_engine_init(NULL, &engine);
  have_engine = engine_init == MA_SUCCESS;
  if (!have_engine) {
    W::log << "Failed to initialize audio engine";
  }
}


// Track
// ------------------------------

struct Track {
  std::string filename;
  xm_data_source* xmds;
  ma_sound* sound;

  Track(std::string filename) {
    engine_init();

    ma_result result;
    std::string path = Audio::full_path(filename);

    xmds = new xm_data_source;
    sound = new ma_sound;

    result = xm_data_source_init_file(path.c_str(), xmds);
    if (result != MA_SUCCESS) {
      W::log << "Failed to initialize data source for file: " << path << "\n";
      W::log << "Error: " << ma_result_description(result) << "\n";
      delete xmds;
      xmds = NULL;
      return;
    }

    result = ma_sound_init_from_data_source(&engine, xmds, 0, NULL, sound);
    if (result != MA_SUCCESS) {
      W::log << "Failed to initialize sound for file: " << path << "\n";
      W::log << "Error: " << ma_result_description(result) << "\n";
      delete sound;
      sound = NULL;
      return;
    }
  }

  ~Track() {
    if (sound) {
      ma_sound_stop(sound);
      ma_sound_uninit(sound);
      delete sound;
    }
    if (xmds) {
      xm_data_source_uninit(xmds);
      delete xmds;
    }
  }
};

Track *current_track = NULL;


// Interface
// ------------------------------

namespace Audio {
  void playSound(std::string filename) {
    engine_init();

    if (have_engine) {
      std::string path = full_path(filename);
      ma_result result = ma_engine_play_sound(&engine, path.c_str(), NULL);
      if (result != MA_SUCCESS) {
        W::log << "Failed to play sound file: " << filename << "\n";
      }
    }
  }

  void playMusic(std::string filename) {
    engine_init();
    if (have_engine) {
      stopMusic();
      current_track = new Track(filename);
      if (current_track->sound) {
        ma_sound_start(current_track->sound);
      }
    }
  }

  bool musicIsPlaying() {
    return (have_engine &&
            current_track != NULL &&
            current_track->sound &&
            !ma_sound_at_end(current_track->sound));
  }

  void stopMusic() {
    if (have_engine && current_track != NULL) {
      delete current_track;
      current_track = NULL;
    }
  }
}

