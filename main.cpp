/*
 * main.cpp - entry point for Linux & Windows Me
 *
 */

#if defined _WIN32 || _WIN64
	#include <windows.h>
#endif

#include "W.h"
#include "Classes/MenuState.hpp"
#include "Classes/MrPaths.hpp"

int APIENTRY WinMain(HINSTANCE _appInstance, HINSTANCE _prev, LPTSTR _cmdline, int nCmdShow) {
	W::setLogFile(MrPaths::desktopPath + "DBTlog.txt");
	W::log << "settings file path: " << MrPaths::settingsPath  << std::endl;
	W::log << "resources path: "     << MrPaths::resourcesPath << std::endl;
	W::log << "desktop path: "       << MrPaths::desktopPath   << std::endl;
	
	W::createWindow(W::size(800,600), "Generic Tycoon Thingy");
	W::pushState(new MenuState());
	W::start();

	return 1;
}
