#ifdef __APPLE__

/* Mac */

#import <Cocoa/Cocoa.h>
#import <Quartz/Quartz.h>

#include <iostream>

#include "WindowManager.hpp"
#include "MyView.h"

struct NativeObjs {
	NSWindow *window;
	MyWindowDelegate *windowDelegate;
	MyView *view;
	NSOpenGLContext *context;
};

WindowManager::WindowManager() : mode(WINDOWED) {
	objs = new NativeObjs();
	
	// Create OpenGL context
	NSOpenGLPixelFormatAttribute attrs[] = { NSOpenGLPFADoubleBuffer, 0 };
	NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
	if (pf == nil)
		throw MsgException("Couldn't get an appropriate pixel format");
	objs->context = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
	if (objs->context == nil)
		throw MsgException("Couldn't create opengl context");
	
	// Create window
	createWindow();
}
WindowManager::~WindowManager() {
	
}

void WindowManager::createWindow() {
	NSRect frame = NSMakeRect(0, 0, 800, 600);
	objs->window = [[NSWindow alloc] initWithContentRect:frame
											   styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
												 backing:NSBackingStoreBuffered
												   defer:NO];
	NSWindowCollectionBehavior coll = [objs->window collectionBehavior];	// Enable lion fullscreenery
	coll |= NSWindowCollectionBehaviorFullScreenPrimary;					// 
	[objs->window setCollectionBehavior:coll];								//
	
	[objs->window center];
	
	NSRect viewRect = NSMakeRect(0, 0, frame.size.width, frame.size.height);
	objs->view = [[MyView alloc] initWithFrame:viewRect];
	
	[objs->window setContentView:objs->view];	// Add view to window
	
	[objs->context setView:objs->view];			// Set view as context’s drawable object
	
	objs->windowDelegate = [[MyWindowDelegate alloc] init];	// Create delegate to handle window close
	[objs->window setDelegate:objs->windowDelegate];
	
	[objs->window makeKeyAndOrderFront:NSApp];
	[objs->window makeFirstResponder:objs->view];
}
void WindowManager::closeWindow() {
	[objs->context clearDrawable];
	[objs->window release];
	[objs->windowDelegate release];
}

bool WindowManager::goWindowed() {
	if (mode == WINDOWED) return true;
	[objs->window toggleFullScreen:nil];
	return true;
}
bool WindowManager::goFullscreen() {
	if (mode == FULLSCREEN) return true;
	[objs->window toggleFullScreen:nil]; 
	return true;
}
void WindowManager::wentWindowed() {
	mode = WINDOWED;
}
void WindowManager::wentFullscreen() {
	mode = FULLSCREEN;
}

void WindowManager::setTitle(const char *t) {
	[objs->window setTitle:[NSString stringWithUTF8String:t]];
}
void WindowManager::enableDrawing() {
	[objs->context makeCurrentContext];
}
void WindowManager::endDrawing() {
	[NSOpenGLContext clearCurrentContext];
	[objs->context flushBuffer];
}

int WindowManager::width() {
	return [objs->view bounds].size.width;
}
int WindowManager::height() {
	return [objs->view bounds].size.height;
}

void* WindowManager::getEvents() {
	return [objs->view getEvents];
}
void WindowManager::clearEvents() {
	[objs->view clearEvents];
}
void* WindowManager::getView() {
	return objs->view;
}
void* WindowManager::getWindow() {
	return objs->window;
}
void WindowManager::frameChanged() {
	[objs->context update];
	if (mode == WINDOWED)
		setBackBufferSize(width(), height());
}
void WindowManager::setBackBufferSize(int _x, int _y) {
	GLint size[] = { _x, _y };
	CGLContextObj ctx = (CGLContextObj) [objs->context CGLContextObj];
	CGLSetParameter(ctx, kCGLCPSurfaceBackingSize, size);
	CGLEnable(ctx, kCGLCESurfaceBackingSize);
}


#elif defined _WIN32 || _WIN64

/* Windows Me */

#include "WindowManager.hpp"

WindowManager::WindowManager(WNDPROC wndProc) : mode(WINDOWED) {
	this->appInstance = GetModuleHandle(NULL);
	if (this->appInstance == NULL)
		throw MsgException("Couldn't get app instance.");

	// Register window class
	int width = 800, height = 600;
	WNDCLASSEX wc;	// Before creating a window, you have to register a class for it

	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Force redraw on resize; has own device context
	wc.lpfnWndProc = wndProc;
	wc.cbClsExtra = 0;			// No extra class memory 
	wc.cbWndExtra = 0;			// No extra window memory 
	wc.hInstance = this->appInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;		// GetStockObject(WHITE_BRUSH) ?
	wc.lpszMenuName = NULL;			// Menu resource name... "MainMenu" ?
	wc.lpszClassName = "DBTWindow";	// Window class name
	wc.hIconSm = (HICON) LoadImage(wc.hInstance, MAKEINTRESOURCE(5), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

	if (!RegisterClassEx(&wc))
		throw MsgException("Failed to register window class.");

	// Set window style & size
	DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	DWORD extendedWindowStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

	RECT rect;
	rect.left = 0, rect.right = width;
	rect.top = 0, rect.bottom = height;
	AdjustWindowRectEx(&rect, windowStyle, FALSE, extendedWindowStyle);

	// Create window
	this->windowHandle = CreateWindowEx(
		extendedWindowStyle,				//
		"DBTWindow",
		"Oh hai",		// title
		windowStyle,
		0, 0,			// position
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,			// parent window
		NULL,			// menu
		this->appInstance,
		NULL			// don't send WM_CREATE
	);
	if(this->windowHandle == NULL) {
		DWORD err = GetLastError();
		char errorMsg[200];
		wsprintf(errorMsg, "Error creating window. Error code: %d, %X.", err, err);
		closeWindow();
		throw MsgException(errorMsg);
	}
	// Get device context
	if (!(this->deviceContext = GetDC(this->windowHandle))) {
		closeWindow();
		throw MsgException("Error creating an OpenGL device context");
	}
	// Create pixel format
	int pf;
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,						// Version number
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // Support windows, opengl, and double buffering
		PFD_TYPE_RGBA,
		16,						// Colour bit depth
		0, 0, 0, 0, 0, 0,		// Color bits (ignored)
		0,						// Alpha buffer
		0,						// Shift bit
		0,						// Accumulation buffer
		0, 0, 0, 0,				// Accumulation bits (ignored)
		16,						// 16 bit depth buffer
		0,						// Stencil buffer
		0,						// Auxiliary buffer
		PFD_MAIN_PLANE,			// Main drawing layer
		0,						// Reserved
		0, 0, 0					// Layer Masks Ignored
	};
	if (!(pf = ChoosePixelFormat(this->deviceContext, &pfd))) {
		closeWindow();
		throw MsgException("Unable to get a suitable pixel format");
	}
	if(!SetPixelFormat(this->deviceContext, pf, &pfd)) {
		closeWindow();
		throw MsgException("Unable to set the pixel format");
	}
	// Create rendering context
	if (!(this->renderingContext = wglCreateContext(this->deviceContext))) {
		closeWindow();
		throw MsgException("Error creating a rendering context");
	}
	// Make rendering context current
	if (!wglMakeCurrent(this->deviceContext, this->renderingContext)) {
		closeWindow();
		throw MsgException("Error activating the rendering context");
	}

	ShowWindow(this->windowHandle, SW_SHOW);
	SetForegroundWindow(this->windowHandle);
	SetFocus(this->windowHandle);
}
WindowManager::~WindowManager() {
	closeWindow();
}

void WindowManager::closeWindow() {
	// If fullscreen, also leave fullsceren mode
	// ...
	if (this->renderingContext) {
		if (!wglMakeCurrent(NULL, NULL))
			MessageBox(NULL, "Error releasing device and rendering contexts", "Error", MB_OK | MB_ICONEXCLAMATION);
		if (!wglDeleteContext(this->renderingContext))
			MessageBox(NULL, "Error deleting the rendering context", "Error", MB_OK | MB_ICONEXCLAMATION);
		this->renderingContext = NULL;
	}
	if (this->deviceContext && !ReleaseDC(this->windowHandle, this->deviceContext))
		MessageBox(NULL, "Error releasing the device context", "Error", MB_OK | MB_ICONEXCLAMATION);
	this->deviceContext = NULL;
	if (this->windowHandle && !DestroyWindow(this->windowHandle))
		MessageBox(NULL, "Error destroying the window", "Error", MB_OK | MB_ICONEXCLAMATION);
	this->windowHandle = NULL;
	if (!UnregisterClass("OpenGL", this->appInstance))
		MessageBox(NULL, "Error unregistering the window class", "Error", MB_OK | MB_ICONEXCLAMATION);
	this->appInstance = NULL;
}

bool WindowManager::goWindowed() {
	if (mode == WINDOWED)
		return true;
	if (mode == FULLSCREEN) {
		if (ChangeDisplaySettings(NULL, 0) != DISP_CHANGE_SUCCESSFUL)
			return false;

		DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		DWORD extendedWindowStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		
		SetWindowLongPtr(this->windowHandle, GWL_STYLE, windowStyle);
		SetWindowLongPtr(this->windowHandle, GWL_EXSTYLE, extendedWindowStyle);

		SetWindowPos(this->windowHandle, HWND_TOP, 0, 0, 800, 640, SWP_SHOWWINDOW);
	}
	ShowWindow(this->windowHandle, SW_SHOW);
	SetForegroundWindow(this->windowHandle);
	SetFocus(this->windowHandle);				// Get keyboard focus
	mode = WINDOWED;
	return true;
}
bool WindowManager::goFullscreen() {
	if (mode == FULLSCREEN)
		return true;
	DEVMODE dm;
	dm.dmSize = sizeof(DEVMODE);
	dm.dmPelsWidth = 800;
	dm.dmPelsHeight = 640;
	dm.dmBitsPerPel = 16;
	dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
	if (ChangeDisplaySettings(&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		return false;
	DWORD windowStyle = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	DWORD extendedWindowStyle = WS_EX_APPWINDOW;
	//ShowCursor(false);
	SetWindowLongPtr(this->windowHandle, GWL_STYLE, windowStyle);
	SetWindowLongPtr(this->windowHandle, GWL_EXSTYLE, extendedWindowStyle);
	SetWindowPos(this->windowHandle, HWND_TOP, 0, 0, 800, 640, SWP_SHOWWINDOW);
	mode = FULLSCREEN;
	return true;
}

void WindowManager::setTitle(const char *t) {
	SetWindowText(this->windowHandle, t);
}

void WindowManager::enableDrawing() {

}
void WindowManager::endDrawing() {

}

int WindowManager::width() {
	RECT rect;
	GetClientRect(this->windowHandle, &rect);
	return rect.right - rect.left;
}
int WindowManager::height() {
	RECT rect;
	GetClientRect(this->windowHandle, &rect);
	return rect.bottom - rect.top;
}

HWND WindowManager::getWindowHandle() {
	return this->windowHandle;
}
HDC WindowManager::getDeviceContext() {
	return this->deviceContext;
}

#endif
