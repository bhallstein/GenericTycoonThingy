/*
 * SettingsManager.hpp - exclusively handles Settings in the game. may in future use another header for read/write data
 *
 * - Adding settings
 *   - Add the Setting itself under "The Settings" in the class declaration:
 *	 - Setting<value return type> name;
 *
 * - Adding defaults
 *   - Setup the Setting's defaults in loadDefaults, also implemented in the header for your convenience.
 */

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <string>
#include <iostream>

#include "Setting.hpp"
#include "LuaHelper.hpp"

class SettingsManager
{
public:
	SettingsManager(char* argv[]);
	// Methods
	void init(char* argv[]); //initialise SM - this includes loading settings or defaults as appropriate
	void save(std::string fileName); // Save external files (Lua)
	//void detect(); 					// Run detection routines for flagged settings

	//The Settings
	Setting<int> ScreenWidth;
	Setting<int> ScreenHeight;
	Setting<bool> FullScreen;
	Setting<int> FramerateLimit;

	//loadDefaults is implemented in the header because it's important
	//This way if we add a setting we only have to come to the header to add both it AND its defaults
	void loadDefaults()
	{
		//Ultimately we will set more than just values! ;)

		//ScreenWidth
		ScreenWidth.value = 800;

		//ScreenHeight
		ScreenHeight.value = 600;

		//FullScreen
		FullScreen.value = false;

		//FramerateLimit
		FramerateLimit.value = 60;
	}

	//loadSettings is implemented in the header because it's important
	//This way if we add a setting we only have to come to the header to add both it AND its defaults AND it's luaRead
	void loadSettings(LuaHelper* mrLua)
	{		
		mrLua->pushtable("settings");
		ScreenWidth.value = mrLua->getSubfield<int>("ScreenWidth","value");
		ScreenHeight.value = mrLua->getSubfield<int>("ScreenHeight","value");
		FullScreen.value = mrLua->getSubfield<bool>("FullScreen","value");
		FramerateLimit.value = mrLua->getSubfield<int>("FramerateLimit","value");
	}
};



#endif
