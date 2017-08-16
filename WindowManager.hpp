/*
 * WindowManager.hpp - Cross-platform window functions
 *
 */

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include "Windows.h"
#include <string>

#include "Classes/types.h"

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
	enum mode_types { WINDOWED, FULLSCREEN} mode;

};

#endif
