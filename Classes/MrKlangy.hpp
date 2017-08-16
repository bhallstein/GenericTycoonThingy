/*
 * MrKlangy.hpp - your helpful audio assistant
 *
 */

#ifndef MrKlangy_H
#define MrKlangy_H

#include "irrKlang.h"

class MrKlangy {
public:
	static void playSound(const char *);
	static void playBGM(const char *, bool loop = true);
	static void stopBGM();
	
	struct init;
	static struct init *initializer;
	
private:
	static bool device_available;
	static irrklang::ISoundEngine *sound_engine;
	static irrklang::ISound *bgm; // Background music sound instance

};

#endif
