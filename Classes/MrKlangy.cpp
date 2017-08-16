#include "MrKlangy.hpp"
#include "MrPaths.hpp"

irrklang::ISoundEngine *MrKlangy::sound_engine;
irrklang::ISound *MrKlangy::bgm;
bool MrKlangy::device_available;

bool disableSounds = true;

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

void MrKlangy::playSound(const char *filename) {
	if (!device_available || disableSounds) return;
	std::string path = MrPaths::resourcesPath + std::string("Data/") + filename;
	sound_engine->play2D(path.c_str(), false);
}
void MrKlangy::playBGM(const char *filename, bool loop) {
	if (!device_available || disableSounds) return;
	stopBGM();
	std::string path = MrPaths::resourcesPath + std::string("Data/") + filename;
	bgm = sound_engine->play2D(path.c_str(), loop, false, true);
}
void MrKlangy::stopBGM() {
	if (bgm) {
		bgm->stop();
		bgm->drop();
		bgm = NULL;
	}
}

MrKlangy::init *MrKlangy::initializer = new MrKlangy::init;
