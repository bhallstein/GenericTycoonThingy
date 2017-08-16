#include "SettingsManager.hpp"

SettingsManager::SettingsManager(W *_theW /*char* argv[]*/) : theW(_theW)
{
	init();
}

void SettingsManager::init(/*char* argv[]*/)
{
	// Fixed path to local settings file
	std::string fileName = theW->pathToSettingsDir();		// Need to catch exception here.
	fileName.append("settings.lua");
	std::cout << "SM: file: " << fileName << std::endl;

	// Instantiate Lua for loading in
	LuaHelper* mrLua = new LuaHelper;

	// Try to open file.
	// If fails, try to create file from defaults.
	// If that fails, throw exception?
	if(mrLua->loadFile(fileName))
	{
		loadDefaults();
		save(fileName);		// throw if unsuccessful? or is a silent error appropriate here?
		std::cout << lua_tostring(mrLua->LuaInstance, -1); // readout error from the stack
	}
	else
	{
		loadSettings(mrLua);
	}
}

void SettingsManager::save(std::string fileName)
{
	// Do this later
}