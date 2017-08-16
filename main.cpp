/*
 * main.cpp - entry point for Linux & Windows Me
 *
 */

#if defined _WIN32 || _WIN64
	#include <windows.h>
#endif

#include "W.h"
#include "Classes/Menu.hpp"

int APIENTRY WinMain(HINSTANCE _appInstance, HINSTANCE _prev, LPTSTR _cmdline, int nCmdShow) {
	W::Window *window = new W::Window();
	Menu *menu = new Menu(window);

	W::pushState(menu);
	W::startGameLoop();

	MSG msg;
	while (!W::_quit) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				W::_quit = true;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	
	delete window;

	return 1;
}
