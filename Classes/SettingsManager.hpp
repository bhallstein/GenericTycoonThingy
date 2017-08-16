/*
 * SettingsManager.hpp - suggest we rename this to something like DataClasses and put ’em all here
 *
 */

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

/*enum SettingType //Available Setting Types
{
	BOOL,INT,RANGE,STRING,COMBO //These match the XML Types atm
};*/

class Setting
{
public:
	Setting();
	~Setting();
	
	std::string type; //What type of setting is this?
	std::string category; //e.g. Graphics?
	int code; //Unique Numeric code for this setting
	std::vector<std::string> args; //Valid Command Line switches
	std::vector<std::string> options; //Valid Options (e.g. for Combos)
	int range[2]; //array [0]=min value [1]=max value ??
	std::string key;
	std::string value;
	//we could add typed value properties too, optionally.
	std::string displayText;
	std::string tooltip;
	bool enabled; //Is this setting available to change?
	bool dev; //Is this a developer setting?
	bool detect; //should this setting be detected on this load?
};

class SettingsManager
{
public:
	SettingsManager();
	~SettingsManager();
	
	// Methods (should maybe be ints to return error codes)
	int load(bool defaults);	// Load from settings.xml & handle overrides from the command line and .cfg file
	void save();				// Save to settings.xml
	void detect(); 				// Run detection routines for settings flagged with Detect=True
	void loadDefaults();		// Load the settings from defaultsettings.xml
	//map<string,string> paraMap(); // The original paraMap function at this time.
	
	// Properties
	//std::map<std::string, int> numberWang; // this is external to the routine now as it should be set up when xml is parsed, and then left available for the session.
	std::map<std::string, Setting> setMap; // Internal Map of settings. Should be protected, and accessed through a getter.

};

#endif
