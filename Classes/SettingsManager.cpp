#include "SettingsManager.hpp"

SettingsManager::SettingsManager(/*char* argv[]*/)
{
	init();
}

void SettingsManager::init(/*char* argv[]*/)
{
	// Fixed path to local settings file
	std::string fileName = "Data/settings.lua";

	// Instantiate Lua for loading in
	LuaHelper* mrLua = new LuaHelper;

	if(mrLua->loadFile(fileName))
	{
		// File load failed - loadDefaults() and write a default file for next time ;)
		loadDefaults();
		save(fileName);
		std::cout << lua_tostring(mrLua->LuaInstance,-1); //readout error from the stack
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