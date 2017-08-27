#import "AppDelegate.h"
#include "W.h"
#include "MenuState.hpp"
#include "MrPaths.hpp"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	W::setLogPath(MrPaths::desktopPath + "GTTlog.txt");
	W::log << "settings file path: " << MrPaths::settingsPath  << std::endl;
	W::log << "resources path: "     << MrPaths::resourcesPath << std::endl;
	W::log << "desktop path: "       << MrPaths::desktopPath   << std::endl;
	
	W::createWindow(W::size(800,600), "Generic Tycoon Thingy");
	W::pushState(new MenuState());
	W::start();
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
	// Insert code here to tear down your application
}

@end
