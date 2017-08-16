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

W::W(WindowManager *_winManager) : winManager(_winManager), opengl_needs_setting_up(true) {
	this->initializePaths();
	logfile.open(logfilePath.c_str());
	log(resourcesPath.c_str());
}
W::~W() {
	std::cout << "w destroy" << std::endl;
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
void W::drawRect(float _x, float _y, float _width, float _height, colour col) {
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
	logfilePath = path;
	logfilePath.append("/Desktop/DBTlog.txt");
	
	// Settings path
	settingsPath = path;
	settingsPath.append("/Library/Application Support/Demon Barber Tycoon");
	
	// Resources path
	[[[NSBundle mainBundle] resourcePath] getCString:path maxLength:MAX_PATH encoding:NSUTF8StringEncoding];
	resourcesPath = path;
	resourcesPath.append("/");
#elif defined _WIN32 || _WIN64
	// Log path
	char path[MAX_PATH] = "";
	SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, path);
	logfilePath = path;
	logfilePath.append("/DBTlog.txt");
	
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
