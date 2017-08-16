#include "MrPaths.hpp"

#ifdef __APPLE__
#include <Cocoa/Cocoa.h>
#endif

#include "W.h"

std::string MrPaths::logfilePath;
std::string MrPaths::settingsPath;
std::string MrPaths::luaPath;
std::string MrPaths::dataPath;

struct MrPaths::init {
	init() {
#ifdef __APPLE__
		// Log path
		char path[MRPATHS_MAX_PATH] = "";
		[NSHomeDirectory() getCString:path maxLength:MRPATHS_MAX_PATH encoding:NSUTF8StringEncoding];
		logfilePath = path;
		logfilePath.append("/Desktop/DBTlog.txt");
		
		// Settings path
		settingsPath = path;
		settingsPath.append("/Library/Application Support/");
		settingsPath.append(TYCOON_NAME);
		settingsPath.append("/");
		
		// Lua path
		[[[NSBundle mainBundle] resourcePath] getCString:path maxLength:MRPATHS_MAX_PATH encoding:NSUTF8StringEncoding];
		std::string resourcesPath = path;
		luaPath = resourcesPath + "/Lua/";
		
		// Data path
		dataPath = resourcesPath + "/Data/";
#elif defined _WIN32 || _WIN64
		// Log path
		char path[MRPATHS_MAX_PATH] = "";
		SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, path);
		logfilePath = path;
		logfilePath.append("/DBTlog.txt");
		
		// Settings path
		SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path);
		settingsPath = path;
		settingsPath.append("/");
		settingsPath.append(TYCOON_NAME);
		settingsPath.append("/");
		
		/* The following are debug-only: need specifying for debug/release using ifdefs */
		
		// Lua path
		GetModuleFileName(0, path, sizeof(path) - 1);
		std::string resourcesPath = path;
		resourcesPath = resourcesPath.substr(0, resourcesPath.rfind("\\"));
		resourcesPath.append("/../Demon Barber Tycoon/");
		luaPath = resourcesPath + "Lua/";
		
		// Data path
		dataPath = resourcesPath + "Data/";
#endif
	}
};

MrPaths::init *MrPaths::initializer = new MrPaths::init;
