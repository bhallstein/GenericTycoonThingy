#include "W.hpp"

#ifdef TARGET_OS_MAC
#import <Cocoa/Cocoa.h>
#import <Quartz/Quartz.h>
#import "MyView.h"
#elif defined _WIN32 || _WIN64
#include <gl\gl.h>
#include <gl\glu.h>
#endif

#include "Classes/Event.hpp"
#include "Classes/View.hpp"

W::W(WindowManager *_winManager) : 
	winManager(_winManager), opengl_needs_setting_up(true)
{

}

W::~W() {
	std::cout << "w destroy" << std::endl;
}

bool W::goWindowed() {
	return winManager->goWindowed();
}
bool W::goFullscreen() {
	return winManager->goFullscreen();
}
bool W::setResolution(int _x, int _y) {
	// ...
	return true;
}
void W::setWindowTitle(const char *t) {
	winManager->setTitle(t);
}

void W::setUpOpenGL() {
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_SCISSOR_TEST);
	opengl_needs_setting_up = false;
}
void W::frameChanged() {
	// ...
}
void W::startDrawing() {
	winManager->enableDrawing();

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
	winManager->endDrawing();
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

void W::addEvent(Event &ev) {
	events.push_back(ev);
}
std::vector<Event>* W::getEvents() {
	return &events;
}
void W::clearEvents() {
	events.clear();
}

//FILE* W::filePointerToResource(std::string s) {
//	int len = 180; char path[len];
//	NSString *_path = [NSString stringWithFormat:@"%@/%s", [[NSBundle mainBundle] resourcePath], s.c_str()];
//	[_path getCString:path maxLength:len encoding:NSUTF8StringEncoding];
//	return fopen(path, "r");
//}
std::string W::pathForResource(std::string s) {
#ifdef TARGET_OS_MAC
	int len = 200; char path[len];
	NSString *_path = [NSString stringWithFormat:@"%@/%s", [[NSBundle mainBundle] resourcePath], s.c_str()];
	[_path getCString:path maxLength:len encoding:NSUTF8StringEncoding];
	return path;
#elif defined _WIN32 || _WIN64
	char appPath[MAX_PATH] = "";
	std::string appDir;
	GetModuleFileName(0, appPath, sizeof(appPath) - 1);
	appDir = appPath;
	appDir = appDir.substr(0, appDir.rfind("\\"));
	appDir.append("Data/");
	appDir.append(s);
	return appDir;
#endif
}
std::string W::pathToSettingsDir() {
#ifdef TARGET_OS_MAC
	// ...
#elif defined _WIN32 || _WIN64
	char dir[] = "%APPDATA%/Demon Barber Tycoon/";
	DWORD dw = GetFileAttributes(dir);
	if (dw != INVALID_FILE_ATTRIBUTES && (dw & FILE_ATTRIBUTE_DIRECTORY)) {
		std::string s = dir;
		return s;
	}
	if (!CreateDirectory(dir, NULL))
		; // throw
	return dir;
#endif
}

int W::width() {
	return winManager->width();
}
int W::height() {
	return winManager->height();
}

/*#ifdef TARGET_OS_MAC
	return [objs->view bounds].size.width;
	return [objs->view bounds].size.height;
#endif*/

/*
struct NativeObjs {
#ifdef TARGET_OS_MAC
	NSWindow *window;
	MyWindowDelegate *windowDelegate;
	MyView *view;
	NSOpenGLContext *context;
#elif defined _WIN32 || _WIN64
	HINSTANCE appInstance;
	HWND windowHandle;
	HGLRC context;
	HDC deviceContext;
#endif
};
*/

/*
	a_lion_is_here = false;
	
	// Create OpenGL context
	NSOpenGLPixelFormatAttribute attrs[] = { NSOpenGLPFADoubleBuffer, 0 };
	NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
	if (pf == nil) {
		std::cout << "couldn't create pf" << std::endl;
		// throw exception
	}
	objs->context = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
	if (objs->context == nil) {
		std::cout << "couldn't create opengl context" << std::endl;
		// throw exception
	}
#elif defined _WIN32 || _WIN64
	objs = new NativeObjs;
	objs->appInstance = GetModuleHandle(NULL);

	createWindow();		// May throw an exception, which should be handled in main.cpp
#endif
*/

/*
#ifdef TARGET_OS_MAC
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
#endif
}
bool W::goFullscreen() {
	if (mode == FULLSCREEN) return true;
#ifdef TARGET_OS_MAC
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
#endif
}
*/
/*
void W::closeWindow() {
	if (mode == NONE) return;
#ifdef TARGET_OS_MAC
	[objs->context clearDrawable];
	[objs->window release];
	[objs->windowDelegate release];
#elif defined _WIN32 || _WIN64
	// ...
#endif
	mode = NONE;
}
*/
/*
void W::setBackBufferSize(int _x, int _y) {
#ifdef TARGET_OS_MAC
	GLint size[] = { _x, _y };
	CGLContextObj ctx = (CGLContextObj) [objs->context CGLContextObj];
	CGLSetParameter(ctx, kCGLCPSurfaceBackingSize, size);
	CGLEnable(ctx, kCGLCESurfaceBackingSize);
#elif defined _WIN32 || _WIN64
	// May not need doing on windows, or something else may need doing... display mode perhaps
#endif
}
*/

/*
void W::frameChanged() {
#ifdef TARGET_OS_MAC
	[objs->context update];
	// Update the backing buffer if in windowed mode
	if (mode == WINDOWED)
		setBackBufferSize(width(), height());
#elif defined _WIN32 || _WIN64
	// ...
#endif
}
*/

/*
#ifdef TARGET_OS_MAC
void* W::getView() {
	return objs->view;
}
#endif
*/

/*void W::setWindowTitle(std::string s) {
#ifdef TARGET_OS_MAC
	[objs->window setTitle:[NSString stringWithUTF8String:s.c_str()]];
#elif defined _WIN32 || _WIN64

#endif
}*/

/*
std::vector<Event*>* W::getEvents() {
#ifdef TARGET_OS_MAC
	events.clear();
	// Convert to DBTEvents
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
			ev->key = convertNativeStringToKeycode([[ext_events objectForKey:event_type] characterAtIndex:0]);
		}
		else
			ev->setType(Event::UNKNOWN_EVENT);
		events.push_back(ev);
	}
	[objs->view clearEvents];
#endif
	// Synthesize a mousemove event from the current mouse position
#ifdef TARGET_OS_MAC
	NSPoint mouseloc = [objs->view convertMouseCoords:[[objs->view window] mouseLocationOutsideOfEventStream]];
#elif defined _WIN32 || _WIN64
	POINT mouseloc;
	GetCursorPos(&mouseloc);
	ScreenToClient(objs->windowHandle, &mouseloc);
#endif
	if (mouseloc.x >= 0 && mouseloc.y >= 0 && mouseloc.x < width() && mouseloc.y < height()) {
		Event *ev = new Event;
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
	
	// Send close event if have been asked to
	if (quit_event) {
		Event *ev = new Event;
		ev->type = Event::CLOSED;
		events.push_back(ev);
		quit_event = false;
	}

	return &events;
}
*/
/*
void W::sendQuitEvent() {
	quit_event = true;
}
*/

/*
void W::startDrawing() {
#ifdef TARGET_OS_MAC
	[objs->context makeCurrentContext];
#elif defined _WIN32 || _WIN64
	events.clear();
#endif
}
void W::finishDrawing() {
#ifdef TARGET_OS_MAC
	[NSOpenGLContext clearCurrentContext];
	[objs->context flushBuffer];
#elif defined _WIN32 || _WIN64
	// Don't think anything needs doing here
#endif
}
*/
