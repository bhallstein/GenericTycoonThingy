#include "W.hpp"

#ifdef _APPLE__
#import <Cocoa/Cocoa.h>
#import <Quartz/Quartz.h>
#import "MyView.h"
#elif defined _WIN32 || _WIN64
#include <gl\gl.h>
#include <gl\glu.h>
#endif

#include "Classes/Event.hpp"
#include "Classes/View.hpp"

W::W(WindowManager *_winManager) : winManager(_winManager), opengl_needs_setting_up(true) {
	
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
#ifdef __APPLE__
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
#ifdef __APPLE__
	return "Oh, hello";
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
