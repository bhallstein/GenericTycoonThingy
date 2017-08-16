#include "MrKlangy.hpp"
#include "MrPaths.hpp"

irrklang::ISoundEngine *MrKlangy::sound_engine;
irrklang::ISound *MrKlangy::bgm;

struct MrKlangy::init {
	init() {
		sound_engine = irrklang::createIrrKlangDevice();
        bgm = 0;
	}
};

void MrKlangy::playSound(const char *soundfile) {
	sound_engine->play2D((MrPaths::dataPath + soundfile).c_str(), false);
}
void MrKlangy::playBGM(const char *soundfile, bool loop) {
	stopBGM();
	bgm = sound_engine->play2D((MrPaths::dataPath + soundfile).c_str(), loop, false, true);
}
void MrKlangy::stopBGM() {
	if (bgm) {
		bgm->stop();
		bgm->drop();
		bgm = NULL;
	}
}

MrKlangy::init *MrKlangy::initializer = new MrKlangy::init;
