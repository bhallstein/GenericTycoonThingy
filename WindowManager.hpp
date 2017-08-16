/*
 * WindowManager.hpp - Cross-platform window functions
 *
 */

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <string>
#include <exception>

#include "Classes/types.h"

#ifdef __APPLE__

/* Mac */

struct NativeObjs;

class WindowManager {
public:
	WindowManager();
	~WindowManager();
	
	// Methods
	void createWindow();
	void closeWindow();
	bool goWindowed();
	bool goFullscreen();
	void setTitle(const char *q);
	
	void enableDrawing();
	void endDrawing();
	
	int width();
	int height();
	
	void* getEvents();
	void clearEvents();
	void* getView();
	void frameChanged();
	void setBackBufferSize(int _w, int _h);
	
protected:
	// Properties
	struct NativeObjs *objs;
	enum mode_types { WINDOWED, FULLSCREEN } mode;
	bool a_lion_is_here;
	
};


#elif defined _WIN32 || _WIN64

/* Windows Me */

#include "Windows.h"

class WindowManager {
public:
	WindowManager(WNDPROC);
	~WindowManager();
	
	// Methods
	void closeWindow();
	bool goWindowed();
	bool goFullscreen();
	void setTitle(const char *q);
	
	void enableDrawing();
	void endDrawing();
	
	int width();
	int height();
	
	HWND getWindowHandle();
	HDC getDeviceContext();
	
	//bool setResolution(int _x, int _y);
	
protected:
	// Properties
	HWND windowHandle;
	HDC deviceContext;
	HGLRC renderingContext;
	HINSTANCE appInstance;
	enum mode_types { WINDOWED, FULLSCREEN } mode;
	
};

#endif

#endif
