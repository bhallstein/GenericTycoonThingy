/*
 * main.cpp - entry point for Linux & Windows Me
 *
 */

#if defined _WIN32 || _WIN64
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#endif

#include <vector>

#include "WindowManager.hpp"
#include "W.hpp"
#include "Classes/Game.hpp"
#include "Classes/Event.hpp"

void dealWithResize(W *_theW) {
	static W *theW;
	if (_theW != NULL) theW = _theW;
	// ...
}
void sendEventToW(Event *ev, W *_theW) {
	static W *theW;
	if (_theW != NULL) theW = _theW;
	else theW->addEvent(*ev);
}

Event::key_code convertNativeStringToKeycode(char c) {
	if (c >= 'a' && c <= 'z') return (Event::key_code) ((int)Event::K_A + c - 'a');
	if (c >= 'A' && c <= 'Z') return (Event::key_code) ((int)Event::K_A + c - 'A');
	if (c >= '0' && c <= '9') return (Event::key_code) ((int)Event::K_0 + c - '0');
	if (c == 27) return Event::K_ESC;		// These are standard ASCII codes
	if (c == 13) return Event::K_RETURN;	//
	if (c == 8)  return Event::K_BACKSPACE;	//
	if (c == 9)  return Event::K_TAB;		//
//#ifdef TARGET_OS_MAC
//	if (c == NSLeftArrowFunctionKey)	return Event::K_LEFT_ARROW;
//	if (c == NSRightArrowFunctionKey)	return Event::K_RIGHT_ARROW;
//	if (c == NSUpArrowFunctionKey)		return Event::K_UP_ARROW;
//	if (c == NSDownArrowFunctionKey)	return Event::K_DOWN_ARROW;
//#elif defined _WIN32 || _WIN64
//	// ...
//#endif
	return Event::K_OTHER;
}
void convertToDBTEvent(Event &ev, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_LBUTTONUP) {
		ev.type = Event::LEFTCLICK;
		ev.x = LOWORD(lParam);
		ev.y = HIWORD(lParam);
	}
	else if (msg == WM_RBUTTONUP) {
		ev.type = Event::RIGHTCLICK;
		ev.x = LOWORD(lParam);
		ev.y = HIWORD(lParam);
	}
	else if (msg == WM_CHAR) {
		ev.type = Event::KEYPRESS;
		ev.key = convertNativeStringToKeycode(wParam);
	}
	else if (msg == WM_KEYDOWN) {
		ev.type = Event::KEYPRESS;
		if (wParam == VK_LEFT)			ev.key = Event::K_LEFT_ARROW;
		else if (wParam == VK_RIGHT)	ev.key = Event::K_RIGHT_ARROW;
		else if (wParam == VK_UP)		ev.key = Event::K_UP_ARROW;
		else if (wParam == VK_DOWN)		ev.key = Event::K_DOWN_ARROW;
		else if (wParam == VK_HOME)		ev.key = Event::K_HOME;
		else if (wParam == VK_END)		ev.key = Event::K_END;
		else if (wParam == VK_DELETE)	ev.key = Event::K_DELETE;
		else ev.key = Event::K_OTHER;
	}
	else if (msg == WM_CLOSE)
		ev.type = Event::CLOSED;
	else
		ev.type = Event::UNKNOWN_EVENT;
}
void synthesizeMousemoveEvent(WindowManager *_winManager) {
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(_winManager->getWindowHandle(), &p);
	int w = _winManager->width(), h = _winManager->height();
	if (p.x >= 0 && p.y >= 0 && p.x < w && p.y < h) {
		Event *ev = new Event;
		ev->setType(Event::MOUSEMOVE);
		ev->x = (int) p.x;
		ev->y = (int) p.y;
		sendEventToW(ev, NULL);
		// Generate screenedge events, used for scrolling the map
		int scrollmargin = 20;
		if (ev->x < scrollmargin) {
			Event *ev = new Event;
			ev->type = Event::SCREENEDGE_LEFT;
			sendEventToW(ev, NULL);
		}
		else if (ev->x >= w - scrollmargin) {
			Event *ev = new Event;
			ev->type = Event::SCREENEDGE_RIGHT;
			sendEventToW(ev, NULL);
		}
		if (ev->y < scrollmargin) {
			Event *ev = new Event;
			ev->type = Event::SCREENEDGE_TOP;
			sendEventToW(ev, NULL);
		}
		else if (ev->y >= h - scrollmargin) {
			Event *ev = new Event;
			ev->type = Event::SCREENEDGE_BOTTOM;
			sendEventToW(ev, NULL);
		}
	}
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE _appInstance, HINSTANCE _prev, LPTSTR _cmdline, int nCmdShow) {
	WindowManager *winManager;
	try {
		winManager = new WindowManager(WndProc);
	}
	catch (MsgException &ex) {
		MessageBox(NULL, ex.msg.c_str(), "Error", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}
	W *theW = new W(winManager);
	Game *game = new Game(theW);

	MSG msg;
	dealWithResize(theW);			// Save static refs to W so WndProc can send things to it
	sendEventToW(NULL, theW);		// 

	BOOL keep_running = true;
	
	while (keep_running) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				keep_running = false;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			synthesizeMousemoveEvent(winManager);
			keep_running = game->update();
			theW->clearEvents();
			if (keep_running) {
				theW->startDrawing();
				game->draw();
				theW->finishDrawing();
				SwapBuffers(winManager->getDeviceContext());
			}
		}
	}

	return 1;
}

LRESULT CALLBACK WndProc(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_LBUTTONUP || msg == WM_RBUTTONUP || msg == WM_CHAR || msg == WM_KEYDOWN || msg ==  WM_CLOSE) {
		Event ev;
		convertToDBTEvent(ev, msg, wParam, lParam);
		sendEventToW(&ev, NULL);
		return 0;
	}
	else if (msg == WM_KEYUP) {
		return 0;
	}
	else if (msg == WM_SIZE) {
		dealWithResize(NULL);	// Seems on win just to need to deal with resize
		return 0;
	}
	else if (msg == WM_ACTIVATE) {
		bool active = HIWORD(wParam) ? false : true;	// May want to un/pause the game loop
		return 0;
	}
	if (msg == WM_SYSCOMMAND && (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER))
		return 0;		// Prevent these from happening
	
	return DefWindowProc(windowHandle, msg, wParam, lParam);
}
