/*
 * Generic Tycoon Thingy
 *
 * ===============
 *  MrPaths.hpp
 * ===============
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#ifndef MrPaths_H
#define MrPaths_H

#define MRPATHS_MAX_PATH 300
#define TYCOON_NAME "Generic Tycoon Thingy"

#include <string>

class MrPaths {
public:
	static std::string desktopPath;
	static std::string settingsPath;
	static std::string resourcesPath;
	
	struct init;						// *
	static struct init *initializer;	//
};

#endif

// * This is a trick to initialize a class's static data members:
//   - create a struct type belonging to the class
//   - the struct's constructor is set to perform the required class initialization
//   - add a static instance of the struct to the class (thereby running the constructor on program load)
