#import "AppDelegate.h"
#include "W.h"
#include "Classes/MenuState.hpp"
#include "Classes/MrPaths.hpp"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	W::setLogFile(MrPaths::desktopPath + "DBTlog.txt");
	W::log << "settings file path: " << MrPaths::settingsPath  << std::endl;
	W::log << "resources path: "     << MrPaths::resourcesPath << std::endl;
	W::log << "desktop path: "       << MrPaths::desktopPath   << std::endl;
	
	W::createWindow(W::size(800,600), "Generic Tycoon Thingy");
	W::pushState(new MenuState());
	W::start();
}

@end
