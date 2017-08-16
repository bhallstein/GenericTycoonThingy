#include "W.hpp"

#ifdef __APPLE__
#import <Cocoa/Cocoa.h>
#import <Quartz/Quartz.h>
#import "MyView.h"
#elif defined _WIN32 || _WIN64
#include <gl\gl.h>
#include <gl\glu.h>
#include "shlobj.h"
#endif

#include "Classes/Event.hpp"
#include "Classes/View.hpp"

std::string W::logfilePath;
std::ofstream W::logfile;

W::W(WindowManager *_winManager) : winManager(_winManager), opengl_needs_setting_up(true) {
	this->initializePaths();
	W::logfile.open(logfilePath.c_str());
#ifdef __APPLE__
	// Tail log file in a Terminal window
	system("echo \"#!/bin/bash\" > ~/Desktop/tail_cmd");
	system("echo \"tail -f ~/Desktop/DBTlog.txt\" >> ~/Desktop/tail_cmd");
	system("chmod a+x ~/Desktop/tail_cmd; open -a Terminal ~/Desktop/tail_cmd");
#endif
	std::string s = "W: settingsPath: "; s.append(settingsPath);
	std::string t = "W: resourcesPath: "; t.append(resourcesPath);
	log(s.c_str()); log(t.c_str()); log("");
}
W::~W() {
	log("W destruct");
#ifdef __APPLE__
	system("mv ~/Desktop/tail_cmd ~/.Trash/");
#endif
	logfile.close();
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
__colour W::stringToColour(std::string &s) {
	__colour c;
	if (s == "black")		c.red = 0,    c.green = 0,    c.blue = 0;
	else if (s == "white")  c.red = 1,    c.green = 1,    c.blue = 1;
	else if (s == "red")    c.red = 1,    c.green = 0,    c.blue = 0;
	else if (s == "green")  c.red = 0,    c.green = 1,    c.blue = 0;
	else if (s == "blue")   c.red = 0,    c.green = 0,    c.blue = 1;
	else if (s == "yellow") c.red = 1,    c.green = 1,    c.blue = 0;
	else if (s == "purple") c.red = 0.47, c.green = 0,    c.blue = 0.78;
	c.alpha = s == "black" ? 0.5 : 1;
	return c;
}
void W::drawRect(float _x, float _y, float _width, float _height, const char *colname) {
	std::string s(colname);
	__colour col = stringToColour(s);
	glColor4f(col.red, col.green, col.blue, col.alpha);

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

void W::initializePaths() {
#ifdef __APPLE__
	// Log path
	char path[MAX_PATH] = "";
	[NSHomeDirectory() getCString:path maxLength:MAX_PATH encoding:NSUTF8StringEncoding];
	W::logfilePath = path;
	W::logfilePath.append("/Desktop/DBTlog.txt");
	
	// Settings path
	settingsPath = path;
	settingsPath.append("/Library/Application Support/Demon Barber Tycoon/");
	
	// Resources path
	[[[NSBundle mainBundle] resourcePath] getCString:path maxLength:MAX_PATH encoding:NSUTF8StringEncoding];
	resourcesPath = path;
	resourcesPath.append("/Data/");
#elif defined _WIN32 || _WIN64
	// Log path
	char path[MAX_PATH] = "";
	SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, path);
	W::logfilePath = path;
	W::logfilePath.append("/DBTlog.txt");
	
	// Settings path
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path);
	settingsPath = path;
	settingsPath.append("/Demon Barber Tycoon/");

	// Resources path
	GetModuleFileName(0, path, sizeof(path) - 1);
	resourcesPath = path;
	resourcesPath = resourcesPath.substr(0, resourcesPath.rfind("\\"));
	resourcesPath.append("/../Demon Barber Tycoon/Data/");		// When installed, will want to be "/Data/" - or...?
#endif
}
bool W::isValidDir(const char *dir) {
#ifdef __APPLE__
	BOOL isdir;
	[[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithCString:dir encoding:NSUTF8StringEncoding]
										 isDirectory:&isdir];
	return isdir;
#elif defined _WIN32 || _WIN64
	DWORD dw = GetFileAttributes(dir);
	return dw != INVALID_FILE_ATTRIBUTES && (dw & FILE_ATTRIBUTE_DIRECTORY);
#endif
}
bool W::createDir(const char *dir) {
#ifdef __APPLE__
	return [[NSFileManager defaultManager] createDirectoryAtPath:[NSString stringWithCString:dir encoding:NSUTF8StringEncoding]
									 withIntermediateDirectories:YES
													  attributes:nil
														   error:nil];
#elif defined _WIN32 || _WIN64
	return CreateDirectory(dir, NULL);
#endif
}

void W::warning(const char *msg, const char *title) {
#ifdef __APPLE__
	NSBeginAlertSheet(
		[NSString stringWithCString:title encoding:NSUTF8StringEncoding],
		@"OK",
		nil, nil, (NSWindow*)winManager->getWindow(), nil, NULL, NULL, NULL,
		[NSString stringWithCString:msg encoding:NSUTF8StringEncoding]
	);
#elif defined _WIN32 || _WIN64
	MessageBox(NULL, msg, title, MB_OK | MB_ICONEXCLAMATION);
#endif
}

void W::log(const char *txt) {
	logfile << txt << std::endl;
}

int W::width() {
	return winManager->width();
}
int W::height() {
	return winManager->height();
}
