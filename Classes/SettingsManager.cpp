#include "SettingsManager.hpp"

SettingsManager::SettingsManager(W *_theW /*char* argv[]*/) : theW(_theW)
{
	bool usedefaults = true;
	filename = theW->settingsPath;
	
	// Check settings directory exists. Try to create it if not. 
	if (!theW->isValidDir(filename.c_str())) {
		if (!theW->createDir(filename.c_str()))
			theW->warning("Could not create settings directory.");
		else {
			filename.append("settings.lua");
			// if (can create file)
			//	usedefaults = false;
			// else
			//	; Try to create file (with defaults), otherwise throw up a message
		}
	}
	// If we have a valid directory, try to load the settings file.
	if (!usedefaults) {
		mrLua = new LuaHelper(theW);
		if (mrLua->loadFile(filename))
			usedefaults = false;
		else 
			theW->warning("Couldn't read the settings file. It looks like it's become corrupted.");
	}
	//std::cout << lua_tostring(mrLua->LuaInstance, -1); // read out error from the stack
}
SettingsManager::~SettingsManager() {
	delete mrLua;
}

void SettingsManager::save(std::string fileName)
{
	// Do this later
}
