#import "DBTAppDelegate.h"
#import "WindowManager.hpp"
#import "W.hpp"
#import "MyView.h"
#import "Classes/Game.hpp"
#import "Classes/Event.hpp"
#import "Classes/Returny.hpp"


Event::key_code charToKeycode(unsigned int c) {
	if (c >= 'a' && c <= 'z') return (Event::key_code) ((int)Event::K_A + c - 'a');
	if (c >= 'A' && c <= 'Z') return (Event::key_code) ((int)Event::K_A + c - 'A');
	if (c >= '0' && c <= '9') return (Event::key_code) ((int)Event::K_0 + c - '0');
	if (c == 27) return Event::K_ESC;		// These are standard ASCII codes
	if (c == 13) return Event::K_RETURN;	//
	if (c == 8)  return Event::K_BACKSPACE;	//
	if (c == 9)  return Event::K_TAB;		//
	if (c == NSLeftArrowFunctionKey)	return Event::K_LEFT_ARROW;
	if (c == NSRightArrowFunctionKey)	return Event::K_RIGHT_ARROW;
	if (c == NSUpArrowFunctionKey)		return Event::K_UP_ARROW;
	if (c == NSDownArrowFunctionKey)	return Event::K_DOWN_ARROW;
	return Event::K_OTHER;
}


@implementation DBTAppDelegate

@synthesize window;

-(BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
	return YES;
}

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	quit = false;
	winManager = new WindowManager();
	theW = new W((WindowManager*)winManager);
	game = new Game((W*)theW);
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(frameChange:)
												 name:NSViewGlobalFrameDidChangeNotification
											   object:(MyView*)((WindowManager*)winManager)->getView()];
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(wentFullscreen:)
												 name:NSWindowDidEnterFullScreenNotification
											   object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(wentWindowed:)
												 name:NSWindowDidExitFullScreenNotification
											   object:nil];
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

-(void)sendEventsToW {
	WindowManager *_winManager = (WindowManager*) winManager;
	W *_theW = (W*) theW;
	NSMutableDictionary *new_events = (NSMutableDictionary*) _winManager->getEvents();
	NSEnumerator *enumerator = [new_events keyEnumerator];
	while (NSString *event_type = [enumerator nextObject]) {
		Event ev;
		if (event_type == @"mouseup") {
			NSPoint p = [[new_events objectForKey:event_type] pointValue];
			ev.setType(Event::LEFTCLICK);
			ev.x = (int) p.x, ev.y = (int) p.y;
		}
		else if (event_type == @"rightmouseup") {
			NSPoint p = [[new_events objectForKey:event_type] pointValue];
			ev.setType(Event::RIGHTCLICK);
			ev.x = (int) p.x, ev.y = (int) p.y;
		}
		else if (event_type == @"keydown") {
			ev.setType(Event::KEYPRESS);
			ev.key = charToKeycode([[new_events objectForKey:event_type] characterAtIndex:0]);
		}
		else
			ev.setType(Event::UNKNOWN_EVENT);
		_theW->addEvent(ev);
	}
	_winManager->clearEvents();
}
-(void)sendQuitEvent {
	Event ev;
	ev.type = Event::CLOSED;
	((W*)theW)->addEvent(ev);
}
-(void)synthesizeMousemoveEvent {
	W* _theW = (W*) theW;
	WindowManager *_winMan = (WindowManager*) winManager;
	int w = _winMan->width(), h = _winMan->height();
	MyView *view = (MyView*) _winMan->getView();
	NSPoint mouseloc = [view convertMouseCoords:[[view window] mouseLocationOutsideOfEventStream]];
	if (mouseloc.x >= 0 && mouseloc.y >= 0 && mouseloc.x < w && mouseloc.y < h) {
		Event ev;
		ev.setType(Event::MOUSEMOVE);
		ev.x = (int) mouseloc.x;
		ev.y = (int) mouseloc.y;
		_theW->addEvent(ev);
		// Generate screenedge events, used for scrolling the map
		int scrollmargin = 20;
		if (ev.x < scrollmargin) {
			ev.type = Event::SCREENEDGE_LEFT;
			_theW->addEvent(ev);
		}
		else if (ev.x >= w - scrollmargin) {
			ev.type = Event::SCREENEDGE_RIGHT;
			_theW->addEvent(ev);
		}
		if (ev.y < scrollmargin) {
			ev.type = Event::SCREENEDGE_TOP;
			_theW->addEvent(ev);
		}
		else if (ev.y >= h - scrollmargin) {
			ev.type = Event::SCREENEDGE_BOTTOM;
			_theW->addEvent(ev);
		}
	}
}

-(void)runGameLoop:(NSTimer*)_timer {
	Game *_game = (Game*) game;
	W *_theW = (W*) theW;
	
	[self synthesizeMousemoveEvent];
	[self sendEventsToW];
	bool keep_running = _game->update();
	_theW->clearEvents();
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

-(void)frameChange:(NSNotification*)notification {
	((WindowManager*)winManager)->frameChanged();
}
-(void)wentFullscreen:(NSNotification*)notification {
	((WindowManager*)winManager)->wentFullscreen();
}
-(void)wentWindowed:(NSNotification*)notification {
	((WindowManager*)winManager)->wentWindowed();
}

-(void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:NSViewGlobalFrameDidChangeNotification
												  object:(MyView*)((WindowManager*)winManager)->getView()];
	[super dealloc];
}

@end
