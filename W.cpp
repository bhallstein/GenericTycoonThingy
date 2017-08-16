#import <Cocoa/Cocoa.h>
#import <Quartz/Quartz.h>

#import "MyView.h"
#include "Event.hpp"

struct NativeObjs {
	NSWindow *window;
	MyWindowDelegate *windowDelegate;
	MyView *view;
	NSOpenGLContext *context;
};

Event::key_code convertNativeStringToKeycode(NSString *str) {
	unichar c = [str characterAtIndex:0];
	if (c >= 'a' && c <= 'z') return (Event::key_code) ((int)Event::K_A + c - 'a');
	if (c >= 'A' && c <= 'Z') return (Event::key_code) ((int)Event::K_A + c - 'A');
	if (c >= '0' && c <= '9') return (Event::key_code) ((int)Event::K_0 + c - '0');
	if (c == NSLeftArrowFunctionKey)	return Event::K_LEFT_ARROW;
	if (c == NSRightArrowFunctionKey)	return Event::K_RIGHT_ARROW;
	if (c == NSUpArrowFunctionKey)		return Event::K_UP_ARROW;
	if (c == NSDownArrowFunctionKey)	return Event::K_DOWN_ARROW;
	if (c == 27) return Event::K_ESC;		// Apparently these are rather universal
	if (c == 13) return Event::K_RETURN;	// 
	return Event::K_OTHER;
}

#include "W.hpp"
#include "Classes/View.hpp"

W::W() : mode(NONE), quit_event(false), opengl_needs_setting_up(true) {
	a_lion_is_here = false;
	objs = new NativeObjs;
	
	// Create OpenGL context
	NSOpenGLPixelFormatAttribute attrs[] = { NSOpenGLPFADoubleBuffer, 0 };
	NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
	if (pf == nil)
		std::cout << "couldn't create pf" << std::endl;	// throw exception
	objs->context = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
	if (objs->context == nil)
		std::cout << "couldn't create opengl context" << std::endl;	// throw exception
}

W::~W() {
	closeWindow();
	delete objs;
	std::cout << "w destroy" << std::endl;
}

bool W::goWindowed() {
	if (mode == WINDOWED) return true;
	
	if (a_lion_is_here) {
		
	}
	else {
		NSRect frame = NSMakeRect(0, 0, 800, 600);
		objs->window = [[NSWindow alloc] initWithContentRect:frame
														styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
														  backing:NSBackingStoreBuffered
															defer:NO];
		[objs->window center];
		
		NSRect viewRect = NSMakeRect(0, 0, frame.size.width, frame.size.height);
		objs->view = [[MyView alloc] initWithFrame:viewRect];
		
		[objs->window setContentView:objs->view];		// Add view to window
		
		[objs->context setView:objs->view];			// Set view as context’s drawable object
		
		objs->windowDelegate = [[MyWindowDelegate alloc] init];	// Create delegate to handle window close
		[objs->window setDelegate:objs->windowDelegate];
		
		[objs->window makeKeyAndOrderFront:NSApp];
		[objs->window makeFirstResponder:objs->view];
	}
	
	mode = WINDOWED;
	return true;
}
bool W::goFullscreen() {
	if (mode == FULLSCREEN) return true;
	
	if (a_lion_is_here) {
		// Lion fullscreen shizzle
	}
	else {
		// Snow leopard fullscreen shizzle
		closeWindow();
		
		NSRect mainDisplayRect = [[NSScreen mainScreen] frame];			// Create fullscreen window
		objs->window = [[NSWindow alloc] initWithContentRect:mainDisplayRect
														styleMask:NSBorderlessWindowMask
														  backing:NSBackingStoreBuffered
															defer:NO];
		[objs->window setLevel:NSMainMenuWindowLevel+1];
		[objs->window setOpaque:YES];
		[objs->window setHidesOnDeactivate:YES];
		
		NSRect viewRect = NSMakeRect(0.0, 0.0, mainDisplayRect.size.width, mainDisplayRect.size.height);
		objs->view = [[MyView alloc] initWithFrame:viewRect];
		[objs->window setContentView:objs->view];	// Add view to window
		
		[objs->context setView:objs->view];			// Set view as context’s drawable object
		
		objs->windowDelegate = [[MyWindowDelegate alloc] init];	// Create delegate to handle window close
		[objs->window setDelegate:objs->windowDelegate];
		
		[objs->window makeKeyAndOrderFront:NSApp];
		[objs->window makeFirstResponder:objs->view];
	}
	
	mode = FULLSCREEN;
	return true;
}
void W::closeWindow() {
	if (mode == NONE) return;
	[objs->context clearDrawable];
	[objs->window release];
	[objs->windowDelegate release];
	mode = NONE;
}
void W::setBackBufferSize(int _x, int _y) {
	GLint size[] = { _x, _y };
	CGLContextObj ctx = (CGLContextObj) [objs->context CGLContextObj];
	CGLSetParameter(ctx, kCGLCPSurfaceBackingSize, size);
	CGLEnable(ctx, kCGLCESurfaceBackingSize);
}
void W::setUpOpenGL() {
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_SCISSOR_TEST);
	opengl_needs_setting_up = false;
}
void W::frameChanged() {
	[objs->context update];
	// Update the backing buffer if in windowed mode
	if (mode == WINDOWED)
		setBackBufferSize(width(), height());
}
void* W::getView() {
	return objs->view;
}

void W::setWindowTitle(std::string s) {
	[objs->window setTitle:[NSString stringWithUTF8String:s.c_str()]];
}

std::vector<Event*>* W::getEvents() {
	// Convert to DBTEvents
	events.clear();
	NSDictionary *ext_events = [objs->view getEvents];
	NSEnumerator *enumerator = [ext_events keyEnumerator];
	while (NSString *event_type = [enumerator nextObject]) {
		Event *ev = new Event;
		if (event_type == @"mouseup") {
			NSPoint p = [[ext_events objectForKey:event_type] pointValue];
			ev->setType(Event::LEFTCLICK);
			ev->x = (int) p.x, ev->y = (int) p.y;
		}
		else if (event_type == @"rightmouseup") {
			NSPoint p = [[ext_events objectForKey:event_type] pointValue];
			ev->setType(Event::RIGHTCLICK);
			ev->x = (int) p.x, ev->y = (int) p.y;
		}
		else if (event_type == @"keydown") {
			ev->setType(Event::KEYPRESS);
			ev->key = convertNativeStringToKeycode([ext_events objectForKey:event_type]);
		}
		else
			ev->setType(Event::UNKNOWN_EVENT);
		events.push_back(ev);
	}
	// Synthesize a mousemove event from the current mouse position
	NSPoint mouseloc = [objs->view convertMouseCoords:[[objs->view window] mouseLocationOutsideOfEventStream]];
	Event *ev = new Event;
	if (mouseloc.x >= 0 && mouseloc.y >= 0 && mouseloc.x < width() && mouseloc.y < height()) {		
		ev->setType(Event::MOUSEMOVE);
		ev->x = (int) mouseloc.x;
		ev->y = (int) mouseloc.y;
		events.push_back(ev);
		
		// Generate screenedge events, used for scrolling the map
		int scrollmargin = 20;
		if (ev->x < scrollmargin) {
			Event *ev = new Event;
			ev->type = Event::SCREENEDGE_LEFT;
			events.push_back(ev);
		}
		else if (ev->x >= width() - scrollmargin) {
			Event *ev = new Event;
			ev->type = Event::SCREENEDGE_RIGHT;
			events.push_back(ev);
		}
		if (ev->y < scrollmargin) {
			Event *ev = new Event;
			ev->type = Event::SCREENEDGE_TOP;
			events.push_back(ev);
		}
		else if (ev->y >= height() - scrollmargin) {
			Event *ev = new Event;
			ev->type = Event::SCREENEDGE_BOTTOM;
			events.push_back(ev);
		}
	}
	if (quit_event) {
		Event *ev = new Event;
		ev->type = Event::CLOSED;
		events.push_back(ev);
		quit_event = false;
	}
	
	[objs->view clearEvents];
	return &events;
}
void W::sendQuitEvent() {
	quit_event = true;
}

int W::width() {
	return [objs->view bounds].size.width;
}
int W::height() {
	return [objs->view bounds].size.height;
}

void W::startDrawing() {
	[objs->context makeCurrentContext];

	if (opengl_needs_setting_up)
		setUpOpenGL();
	
	int w = width(), h = height();

	current_drawn_view = NULL;
	glScissor(0, 0, w, h);
	
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, h, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	
	glClearColor(0.525, 0.187, 0.886, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}
void W::finishDrawing() {
	[NSOpenGLContext clearCurrentContext];
	[objs->context flushBuffer];
}
void W::setUpDrawingForView(View *v) {
	current_drawn_view = v;
	glScissor(v->x, height() - v->y - v->height, v->width, v->height);
}

void W::drawRect(float _x, float _y, float _width, float _height, colour_name col) {
	if (col == RED)			glColor3f(1, 0, 0);
	else if (col == GREEN)	glColor3f(0, 1, 0);
	else if (col == BLUE)	glColor3f(0, 0, 1);
	else if (col == YELLOW)	glColor3f(1, 1, 0);
	else if (col == WHITE)	glColor3f(1, 1, 1);
	else if (col == BLACK)	glColor4f(0, 0, 0, 0.5);

	float x = _x, y = _y;
	if (current_drawn_view != NULL)
		x += current_drawn_view->x, y += current_drawn_view->y;
	
	glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + _width, y);
		glVertex2f(x + _width, y + _height);
		glVertex2f(x, y + _height);
	glEnd();
}

//FILE* W::filePointerToResource(std::string s) {
//	int len = 180; char path[len];
//	NSString *_path = [NSString stringWithFormat:@"%@/%s", [[NSBundle mainBundle] resourcePath], s.c_str()];
//	[_path getCString:path maxLength:len encoding:NSUTF8StringEncoding];
//	return fopen(path, "r");
//}
std::string W::pathForResource(std::string s) {
	int len = 180; char path[len];
	NSString *_path = [NSString stringWithFormat:@"%@/%s", [[NSBundle mainBundle] resourcePath], s.c_str()];
	[_path getCString:path maxLength:len encoding:NSUTF8StringEncoding];
	return path;
}
