/*
 * Setting.hpp - Contains declaration and implementation (?) of the Template Class "Setting"
 *
 */

#ifndef SETTING_H
#define SETTING_H

#include <string>
#include <vector>

template <class T>
class Setting
{
public: //this may need changing?
	std::string category; //e.g. Graphics?
	int code; //Unique Numeric code for this setting
	std::vector<std::string> args; //Valid Command Line switches
	std::vector<std::string> options; //Valid Options (e.g. for Combos)
	int range[2]; //array [0]=min value [1]=max value ??
	T value; //return different types for value :)
	//std::string displayType; //enum this actually. not used yet; when we have an options menu
	std::string displayText;
	std::string tooltip;
	bool enabled; //Is this setting available to change?
	bool dev; //Is this a developer setting?
	bool detect; //should this setting be detected on this load?
	bool overridden; //has this setting been overridden by cfg/cmdline?
};

#endif
