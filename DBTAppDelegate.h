/*
 * DBTAppDelegate.h - roughly speaking, the Cocoa app’s controller
 *
 * Instantiates W and Game, and creates the timer that runs the Game loop by calling Game::run()
 *
 */

#import <Cocoa/Cocoa.h>

@interface DBTAppDelegate : NSObject <NSApplicationDelegate> {
	void *winManager;
	void *theW;
	void *game;
	NSTimer *timer;
	bool quit;
}
-(void)sendEventsToW;
-(void)sendQuitEvent;
-(void)synthesizeMousemoveEvent;
@property (assign) IBOutlet NSWindow *window;
@end
