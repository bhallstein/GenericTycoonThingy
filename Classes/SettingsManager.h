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
	
	std::string Type; //What type of setting is this?
	std::string Category; //e.g. Graphics?
	int Code; //Unique Numeric code for this setting
	std::vector<std::string> Args; //Valid Command Line switches
	std::vector<std::string> Options; //Valid Options (e.g. for Combos)
	int Range[2]; //array [0]=min value [1]=max value ??
	std::string Key;
	std::string Value;
	//we could add typed value properties too, optionally.
	std::string DisplayText;
	std::string Tooltip;
	bool Enabled; //Is this setting available to change?
	bool Dev; //Is this a developer setting?
	bool Detect; //should this setting be detected on this load?
};

class SettingsManager
{
public:
	SettingsManager(); //con
	~SettingsManager();//decon
	
	std::map<std::string,int> numberWang; //this is external to the routine now as it should be set up when xml is parsed, and then left available for the session.
	std::map<std::string,Setting> SetMap; //Internal Map of stored settings. should be private or protected later, and accessed through a public Get()
	
	//these should maybe be ints in order to return error codes
	int Load(bool defaults); //Load the settings from settings.xml and handle overrides from the command line and .cfg file.
	void Save(); //Save the current (modified?) settings to settings.xml for the future. Does not disable active .cfg/command line overrides for future sessions.
	void Detect(); //Run detection routines for settings flagged with Detect=True
	void LoadDefaults(); //Load the settings from defaultsettings.xml
	
	//map<string,string> paraMap(); //this is the original paraMap function at this time.
};

#endif
