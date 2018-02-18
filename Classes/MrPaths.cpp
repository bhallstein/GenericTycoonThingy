/*
 * Generic Tycoon Thingy
 *
 * ==============
 *  MrPaths.cpp
 * ==============
 *
 * Copyright (C) 2012 - Ben Hallstein
 * All rights reserved
 *
 */

#include "MrPaths.hpp"

#ifdef __APPLE__
	#include <Cocoa/Cocoa.h>
#elif defined _WIN32 || _WIN64
	#include "shlobj.h"
#endif

#include "W.h"

std::string MrPaths::desktopPath;
std::string MrPaths::settingsPath;
std::string MrPaths::resourcesPath;

struct MrPaths::init {
	init() {
		using std::string;
#ifdef __APPLE__
		// Desktop path
		char path[MRPATHS_MAX_PATH] = "";
		[NSHomeDirectory() getCString:path maxLength:MRPATHS_MAX_PATH encoding:NSUTF8StringEncoding];
		desktopPath = string(path) + "/Desktop/";
		
		// Settings path
		settingsPath = path;
		settingsPath.append("/Library/Application Support/");
		settingsPath.append(TYCOON_NAME);
		settingsPath.append("/");
		
		// Top level resources path
		[[[NSBundle mainBundle] resourcePath] getCString:path maxLength:MRPATHS_MAX_PATH encoding:NSUTF8StringEncoding];
		resourcesPath = string(path) + "/";
		
#elif defined _WIN32 || _WIN64
		// Log path
		char path[MRPATHS_MAX_PATH] = "";
		SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, path);
		desktopPath = string(path) + "/";
		
		// Settings path
		SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path);
		settingsPath = path;
		settingsPath.append("/");
		settingsPath.append(TYCOON_NAME);
		settingsPath.append("/");
		
		/* The following are debug-only: need specifying for debug/release using ifdefs */
		
		// Resources path
		GetModuleFileName(0, path, sizeof(path) - 1);
		resourcesPath = path;
		resourcesPath = resourcesPath.substr(0, resourcesPath.rfind("\\"));
		resourcesPath.append("/../Demon Barber Tycoon/");
#endif
	}
};

MrPaths::init *MrPaths::initializer = new MrPaths::init;
