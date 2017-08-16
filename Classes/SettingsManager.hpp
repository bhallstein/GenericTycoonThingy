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
//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/xml_parser.hpp>
//#include <boost/foreach.hpp>

#include "Setting.hpp"

class SettingsManager
{
public:
	SettingsManager(char* argv[]);
	// Methods
	void init(char* argv[]); //initialise SM - this includes loading settings or defaults as appropriate
	void save(std::string fileName); // Save external files (XML and Binary)
	//void detect(); 					// Run detection routines for flagged settings

	//The Settings
	Setting<int> Screen_Width;
	Setting<int> Screen_Height;
	Setting<bool> Windowed;
	Setting<int> FramerateLimit;

	//loadDefaults is implemented in the header because it's important
	//This way if we add a setting we only have to come to the header to add both it AND its defaults
	void loadDefaults()
	{
		//Ultimately we will set more than just values! ;)

		//Screen_Width
		Screen_Width.value = 1280;

		//Screen_Height
		Screen_Height.value = 720;

		//Windowed
		Windowed.value = true;

		//FramerateLimit
		FramerateLimit.value = 60;
	}

};



#endif
