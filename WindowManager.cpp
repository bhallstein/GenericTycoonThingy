#include "WindowManager.hpp"

WindowManager::WindowManager(WNDPROC wndProc) : mode(WINDOWED) {
	this->appInstance = GetModuleHandle(NULL);
	if (this->appInstance == NULL) {
		MessageBox(NULL, "Couldn't get app instance.", "Error", MB_OK | MB_ICONEXCLAMATION);
		// throw
	}

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

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Failed to register window class.", "Error", MB_OK | MB_ICONEXCLAMATION);
		// throw
	}

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
		char errorBuffer[200];
		wsprintf(errorBuffer, "Error creating window. Error code: %d, %X.", err, err);
		MessageBox(NULL, errorBuffer, "Error", MB_ICONEXCLAMATION);
		closeWindow();
		// throw
	}
	// Get device context
	if (!(this->deviceContext = GetDC(this->windowHandle))) {
		MessageBox(NULL, "Error creating an OpenGL device context", "Error", MB_OK | MB_ICONEXCLAMATION);
		closeWindow();
		// throw
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
		MessageBox(NULL, "Unable to get a suitable pixel format", "Error", MB_OK | MB_ICONEXCLAMATION);
		closeWindow();
		// throw
	}
	if(!SetPixelFormat(this->deviceContext, pf, &pfd)) {
		MessageBox(NULL, "Unable to set the pixel format", "Error", MB_OK | MB_ICONEXCLAMATION);
		closeWindow();
		// throw
	}
	// Create rendering context
	if (!(this->renderingContext = wglCreateContext(this->deviceContext))) {
		MessageBox(NULL, "Error creating a rendering context", "Error", MB_OK | MB_ICONEXCLAMATION);
		closeWindow();
		// throw
	}
	// Make rendering context current
	if (!wglMakeCurrent(this->deviceContext, this->renderingContext)) {
		MessageBox(NULL, "Error activating the rendering context", "Error", MB_OK | MB_ICONEXCLAMATION);
		closeWindow();
		// throw
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
