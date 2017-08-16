#import "AppDelegate.h"
#include "W.h"
#include "Classes/Menu.hpp"
#include "Classes/MrPaths.hpp"

struct myObjs {
	W::Window *window;
	Menu *menu;
};

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{	
	W::log << "log file path: "       << MrPaths::logfilePath  << std::endl;
	W::log << "settings file path: "  << MrPaths::settingsPath << std::endl;
	W::log << "lua directory path: "  << MrPaths::luaPath      << std::endl;
	W::log << "data directory path: " << MrPaths::dataPath     << std::endl;
	
	objs = new myObjs();
	objs->window = new W::Window();
	objs->window->setTitle("Generic Tycoon Thingy");

	objs->menu = new Menu(objs->window);
	W::pushState(objs->menu);

	W::startGameLoop();
}

@end
