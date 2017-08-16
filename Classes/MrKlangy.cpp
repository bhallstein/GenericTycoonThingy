#include "MrKlangy.hpp"
#include "MrPaths.hpp"

irrklang::ISoundEngine *MrKlangy::sound_engine;
irrklang::ISound *MrKlangy::bgm;
bool MrKlangy::device_available;

struct MrKlangy::init {
	init() {
		bgm = NULL;
		
		irrklang::ISoundDeviceList* deviceList = irrklang::createSoundDeviceList();
		device_available = deviceList->getDeviceCount();
		deviceList->drop();
		
		if (device_available)
			sound_engine = irrklang::createIrrKlangDevice();
	}
};

void MrKlangy::playSound(const char *soundfile) {
	if (!device_available) return;
	sound_engine->play2D((MrPaths::dataPath + soundfile).c_str(), false);
}
void MrKlangy::playBGM(const char *soundfile, bool loop) {
	if (!device_available) return;
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
