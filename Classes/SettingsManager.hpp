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

#include "../W.hpp"
#include "Setting.hpp"
#include "LuaHelper.hpp"

class SettingsManager
{
public:
	SettingsManager(W *_theW /*char* argv[]*/);

	// Properties
	W *theW;

	Setting<int> screenWidth;
	Setting<int> screenHeight;
	Setting<bool> fullscreen;
	Setting<int> framerateLimit;
	
	// Methods
	void init(/*char* argv[]*/);		// Initialise SM - this includes loading settings or defaults as appropriate
	void save(std::string fileName);	// Save external files (Lua)
	//void detect(); 					// Run detection routines for flagged settings

	// loadDefaults is implemented in the header because it's important
	// This way if we add a setting we only have to come to the header to add both it AND its defaults
	void loadDefaults()
	{
		// Ultimately we will set more than just values! ;)
		screenWidth.value = 800;
		screenHeight.value = 600;
		fullscreen.value = false;
		framerateLimit.value = 60;
	}

	// loadSettings is implemented in the header because it's important
	// This way if we add a setting we only have to come to the header to add both it AND its defaults AND its luaRead
	void loadSettings(LuaHelper* mrLua)
	{		
		mrLua->pushtable("settings");
		screenWidth.value = mrLua->getSubfield<int>("ScreenWidth","value");
		screenHeight.value = mrLua->getSubfield<int>("ScreenHeight","value");
		fullscreen.value = mrLua->getSubfield<bool>("FullScreen","value");
		framerateLimit.value = mrLua->getSubfield<int>("FramerateLimit","value");
	}
};

#endif
