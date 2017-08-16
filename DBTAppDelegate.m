#import "DBTAppDelegate.h"
#import "W.hpp"
#import "MyView.h"
#import "Classes/Game.hpp"
#import "Classes/Returny.hpp"

@implementation DBTAppDelegate


@synthesize window;

-(BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
	return YES;
}

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	quit = false;
	NSLog(@"Creating W");
	theW = new W();
	NSLog(@"Creating Game");
	game = new Game((W*)theW);
	
	NSLog(@"Setting up frame change notification");
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(frameChange:)
												 name:NSViewGlobalFrameDidChangeNotification
											   object:(MyView*)((W*)theW)->getView()];
	NSLog(@"Scheduling timer");
	timer = [NSTimer scheduledTimerWithTimeInterval:1./40.
											 target:self
										   selector:@selector(runGameLoop:)
										   userInfo:nil
											repeats:YES];
}

-(NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender {
	if (quit) return NSTerminateNow;
	[self sendQuitEvent];
	return NSTerminateCancel;
}

-(void)sendQuitEvent {
	((W*)theW)->sendQuitEvent();
}

-(void)runGameLoop:(NSTimer*)_timer {
	Game *_game = (Game*) game;
	W *_theW = (W*) theW;
	
	bool keep_running = _game->update();
	if (keep_running) {
		_theW->startDrawing();
		_game->draw();
		_theW->finishDrawing();
	}
	else {
		[timer invalidate];
		quit = true;
		[NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0];
	}
}

-(void)frameChange:(NSNotificationCenter*)notification {
	((W*)theW)->frameChanged();
}

-(void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:NSViewGlobalFrameDidChangeNotification
												  object:self];
	[super dealloc];
}

@end
