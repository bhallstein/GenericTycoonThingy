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

#include <ctime>

#include "Classes/Event.hpp"
#include "Classes/View.hpp"

MTRand_int32 W::twister;

std::string W::logfilePath;
std::ofstream W::logfile;

W::W(WindowManager *_winManager) : winManager(_winManager), opengl_needs_setting_up(true) {
	time_t timey;
	time(&timey);
	twister.seed(timey);
	
	this->initializePaths();
	W::logfile.open(logfilePath.c_str());
	
	std::string s = "W settingsPath: "; s += settingsPath; s += "\n";
	s += "W luaPath: "; s += luaPath; s += "\n";
	log(s.c_str());
	
	sound_engine = irrklang::createIrrKlangDevice();
}
W::~W() {
	log("W destruct");
	logfile.close();
	sound_engine->drop();
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
inline bool streq(const char *a, const char *b) {
	return !strcmp(a, b);
}
__colour W::stringToColour(const char *s) {
	__colour c;
	if      (streq(s, "black" )) c.red = 0,    c.green = 0,    c.blue = 0;
	else if (streq(s, "white" )) c.red = 1,    c.green = 1,    c.blue = 1;
	else if (streq(s, "red"   )) c.red = 1,    c.green = 0,    c.blue = 0;
	else if (streq(s, "green" )) c.red = 0,    c.green = 1,    c.blue = 0;
	else if (streq(s, "blue"  )) c.red = 0,    c.green = 0,    c.blue = 1;
	else if (streq(s, "yellow")) c.red = 1,    c.green = 1,    c.blue = 0;
	else if (streq(s, "purple")) c.red = 0.47, c.green = 0,    c.blue = 0.78;
	c.alpha = streq(s, "black") ? 0.5 : 1;
	return c;
}
void W::drawRect(float x, float y, float w, float h, const char *colname, float rot) {
	__colour col = stringToColour(colname);
	glColor4f(col.red, col.green, col.blue, col.alpha);
	
	if (current_drawn_view != NULL)
		x += current_drawn_view->x, y += current_drawn_view->y;
	
	glLoadIdentity();
	glTranslatef(x + w/2, y + h/2, 0);
	glRotatef(rot, 0, 0, 1);
	glBegin(GL_QUADS);
		glVertex2f(-w/2, -h/2);
		glVertex2f(w/2, -h/2);
		glVertex2f(w/2, h/2);
		glVertex2f(-w/2, h/2);
	glEnd();
}
void W::drawText(float x, float y, const char *colname, char *text, bool rAlign) {
	int char_width = 14;
	// work out total width & hence position
	int tw = 0, c;
	for (int i=0; (c = text[i]); i++)
		if (c == 'I' | c == 'i' || c == '1' || c == ':') tw += char_width - 4;
		else if (c == 'L' || c == 'l')                   tw += char_width - 2;
		else                                             tw += char_width;
	if (rAlign) x -= tw;
	
	for (int i=0; text[i] != '\0'; i++) {
		char c = text[i];
		switch(c) {
			case 'A' : case 'a' : {
				drawRect(x, y, 10, 2, colname);
				drawRect(x, y+2, 2, 8, colname);
				drawRect(x+2, y+4, 6, 2, colname);
				drawRect(x+8, y+2, 2, 8, colname);
				break;
			}
			case 'B' : case 'b' : {
				drawRect(x, y, 2, 10, colname);
				drawRect(x+2, y, 6, 2, colname);
				drawRect(x+2, y+4, 6, 2, colname);
				drawRect(x+2, y+8, 6, 2, colname);
				drawRect(x+8, y+2, 2, 8, colname);
				break;
			}
			case 'C' : case 'c' : {
				drawRect(x, y, 2, 10, colname);
				drawRect(x+2, y, 7, 2, colname);
				drawRect(x+2, y+8, 7, 2, colname);
				break;
			}
			case 'D': case 'd' : { 
				drawRect(x, y, 2, 10, colname);
				drawRect(x+8, y+2, 2, 8, colname);
				drawRect(x+2, y, 6, 2, colname);
				drawRect(x+2, y+8, 6, 2, colname);
				break;
			}
			case 'E' : case 'e' : {
				drawRect(x, y, 2, 10, colname);
				drawRect(x+2, y, 7, 2, colname);
				drawRect(x+2, y+4, 5, 2, colname);
				drawRect(x+2, y+8, 7, 2, colname);
				break;
			}
			case 'F' : case 'f' : {
				drawRect(x, y, 2, 10, colname);
				drawRect(x+2, y, 7, 2, colname);
				drawRect(x+2, y+4, 5, 2, colname);
				break;
			}
			case 'G' : case 'g' : {
				drawRect(x, y, 2, 10, colname);
				drawRect(x+2, y, 8, 2, colname);
				drawRect(x+2, y+8, 8, 2, colname);
				drawRect(x+8, y+4, 2, 4, colname);
				break;
			}
			case 'H' : case 'h' : { 
				drawRect(x, y, 2, 10, colname);
				drawRect(x+8, y, 2, 10, colname);
				drawRect(x+2, y+4, 6, 2, colname);
				break;
			}
			case '1' : {
				drawRect(x, y, 2, 2, colname);
			}
			case 'I' : case 'i' : {
				drawRect(x+2, y, 2, 10, colname);
				break;
			}
			case 'J' : case 'j' : {
				drawRect(x+6, y, 2, 10, colname);
				drawRect(x, y+8, 6, 2, colname);
				break;
			}
			case 'K' : case 'k' : {
				drawRect(x, y, 2, 10, colname);
				drawRect(x+2, y+4, 5, 2, colname);
				drawRect(x+7, y, 2, 4, colname);
				drawRect(x+7, y+6, 2, 4, colname);
				break;
			}
			case 'L' : case 'l' : {
				drawRect(x+1, y, 2, 10, colname);
				drawRect(x+3, y+8, 6, 2, colname);
				break;
			}
			case 'M' : case 'm' : {
				drawRect(x, y, 2, 10, colname);
				drawRect(x+2, y+2, 2, 2, colname);
				drawRect(x+4, y+4, 2, 2, colname);
				drawRect(x+6, y+2, 2, 2, colname);
				drawRect(x+8, y, 2, 10, colname);
				break;
			}
			case 'N' : case 'n' : {
				drawRect(x, y, 2, 10, colname);
				drawRect(x+2, y+2, 2, 2, colname);
				drawRect(x+4, y+4, 2, 2, colname);
				drawRect(x+6, y+6, 2, 2, colname);
				drawRect(x+8, y, 2, 10, colname);
				break;
			}
			case '0' : {
				drawRect(x+4, y+4, 2, 2, colname);
			}
			case 'O' : case 'o' : {
				drawRect(x, y, 2, 10, colname);
				drawRect(x+8, y, 2, 10, colname);
				drawRect(x+2, y, 6, 2, colname);
				drawRect(x+2, y+8, 6, 2, colname);
				break;
			}
			case 'P' : case 'p' : {
				drawRect(x, y, 2, 10, colname);
				drawRect(x+2, y, 6, 2, colname);
				drawRect(x+2, y+4, 6, 2, colname);
				drawRect(x+8, y, 2, 6, colname);
				break;
			}
			case 'Q' : case 'q' : {
				drawRect(x, y, 2, 10, colname);
				drawRect(x+8, y, 2, 10, colname);
				drawRect(x+2, y, 6, 2, colname);
				drawRect(x+2, y+8, 6, 2, colname);
				drawRect(x+6, y+10, 2, 2, colname);
				break;
			}
			case 'R' : case 'r' : {
				drawRect(x, y, 2, 10, colname);
				drawRect(x+2, y, 6, 2, colname);
				drawRect(x+2, y+4, 6, 2, colname);
				drawRect(x+8, y, 2, 6, colname);
				drawRect(x+6, y+6, 2, 2, colname);
				drawRect(x+8, y+8, 2, 2, colname);
				break;
			}
			case 'S' : case 's' : case '5' : {
				drawRect(x, y, 10, 2, colname);
				drawRect(x, y+4, 10, 2, colname);
				drawRect(x, y+8, 10, 2, colname);
				drawRect(x, y+2, 2, 2, colname);
				drawRect(x+8, y+6, 2, 2, colname);
				break;
			}
			case 'T' : case 't' : {
				drawRect(x, y, 10, 2, colname);
				drawRect(x+4, y+2, 2, 8, colname);
				break;
			}
			case 'U' : case 'u' : {
				drawRect(x, y, 2, 10, colname);
				drawRect(x+8, y, 2, 10, colname);
				drawRect(x+2, y+8, 6, 2, colname);
				break;
			}
			case 'V' : case 'v' : {
				drawRect(x, y, 2, 6, colname);
				drawRect(x+2, y+6, 2, 2, colname);
				drawRect(x+4, y+8, 2, 2, colname);
				drawRect(x+6, y+6, 2, 2, colname);
				drawRect(x+8, y, 2, 6, colname);
				break;
			}
			case 'W' : case 'w' : {
				drawRect(x, y, 2, 8, colname);
				drawRect(x+4, y, 2, 8, colname);
				drawRect(x+8, y, 2, 8, colname);
				drawRect(x+2, y+8, 2, 2, colname);
				drawRect(x+6, y+8, 2, 2, colname);
				break;
			}
			case 'X' : case 'x' : {
				drawRect(x, y, 2, 2, colname);
				drawRect(x+2, y+2, 2, 2, colname);
				drawRect(x+4, y+4, 2, 2, colname);
				drawRect(x+6, y+6, 2, 2, colname);
				drawRect(x+8, y+8, 2, 2, colname);
				drawRect(x+8, y, 2, 2, colname);
				drawRect(x+6, y+2, 2, 2, colname);
				drawRect(x+2, y+6, 2, 2, colname);
				drawRect(x, y+8, 2, 2, colname);
				break;
			}
			case 'Y' : case 'y' : {
				drawRect(x+8, y, 2, 10, colname);
				drawRect(x, y, 2, 2, colname);
				drawRect(x+2, y+2, 2, 2, colname);
				drawRect(x+4, y+4, 4, 2, colname);
				break;
			}
			case 'Z' : case 'z' : {
				drawRect(x, y, 10, 2, colname);
				drawRect(x, y+8, 10, 2, colname);
				drawRect(x+6, y+2, 2, 2, colname);
				drawRect(x+4, y+4, 2, 2, colname);
				drawRect(x+2, y+6, 2, 2, colname);
				break;
			}
			case '2' : {
				drawRect(x, y, 2, 4, colname);
				drawRect(x+2, y, 8, 2, colname);
				drawRect(x+8, y+2, 2, 2, colname);
				drawRect(x+2, y+4, 8, 2, colname);
				drawRect(x, y+6, 2, 4, colname);
				drawRect(x+2, y+8, 8, 2, colname);
				break;
			}
			case '3' : {
				drawRect(x+8, y, 2, 10, colname);
				drawRect(x, y, 8, 2, colname);
				drawRect(x, y+8, 8, 2, colname);
				drawRect(x+2, y+4, 6, 2, colname);
				break;
			}
			case '4' : {
				drawRect(x+8, y, 2, 10, colname);
				drawRect(x, y, 2, 4, colname);
				drawRect(x, y+4, 8, 2, colname);
				break;
			}
			case '6' : {
				drawRect(x, y, 2, 10, colname);
				drawRect(x+2, y, 8, 2, colname);
				drawRect(x+2, y+4, 8, 2, colname);
				drawRect(x+2, y+8, 8, 2, colname);
				drawRect(x+8, y+6, 2, 2, colname);
				break;
			}
			case '7' : {
				drawRect(x, y, 10, 2, colname);
				drawRect(x+8, y+2, 2, 2, colname);
				drawRect(x+6, y+4, 2, 2, colname);
				drawRect(x+4, y+6, 2, 2, colname);
				drawRect(x+2, y+8, 2, 2, colname);
				break;
			}
			case '8' : {
				drawRect(x, y, 2, 10, colname);
				drawRect(x+8, y, 2, 10, colname);
				drawRect(x+2, y, 6, 2, colname);
				drawRect(x+2, y+4, 6, 2, colname);
				drawRect(x+2, y+8, 6, 2, colname);
				break;
			}
			case '9' : {
				drawRect(x+8, y, 2, 10, colname);
				drawRect(x, y, 8, 2, colname);
				drawRect(x, y+4, 8, 2, colname);
				drawRect(x, y+2, 2, 2, colname);
				drawRect(x, y+8, 8, 2, colname);
				break;
			}
//			case '$' : {
//				drawRect(x, y, 10, 2, colname);
//				drawRect(x, y+4, 10, 2, colname);
//				drawRect(x, y+8, 10, 2, colname);
//				drawRect(x, y+2, 2, 2, colname);
//				drawRect(x+8, y+6, 2, 2, colname);
//				drawRect(x+4, y-1, 2, 12, colname);
//				break;
//			}
			case MR_CURRENCY /* £ */ : {
				drawRect(x+4, y, 6, 2, colname);
				drawRect(x+2, y+2, 2, 2, colname);
				drawRect(x, y+4, 8, 2, colname);
				drawRect(x+2, y+6, 2, 4, colname);
				drawRect(x, y+8, 10, 2, colname);
				break;
			}
			case ':' : {
				drawRect(x+2, y+4, 2, 2, colname);
				drawRect(x+2, y+8, 2, 2, colname);
				break;
			}
			default: break;
		}
		if (c == 'I' | c == 'i' || c == '1' || c == ':') x += char_width - 4;
		else if (c == 'L' || c == 'l')                   x += char_width - 2;
		else                                             x += char_width;
	}
}

void W::playSound(const char *soundfile) {
	sound_engine->play2D((dataPath + soundfile).c_str(), false);
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
	
	// Lua path
	[[[NSBundle mainBundle] resourcePath] getCString:path maxLength:MAX_PATH encoding:NSUTF8StringEncoding];
	std::string resourcesPath = path;
	luaPath = resourcesPath + "/Lua/";
	
	// Data path
	dataPath = resourcesPath + "/Data/";
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
	
	// Lua path
	GetModuleFileName(0, path, sizeof(path) - 1);
	std::string resourcesPath = path;
	resourcesPath = resourcesPath.substr(0, resourcesPath.rfind("\\"));

#ifdef _DEBUG //debug builds adjust to VS folder structure; Release builds use /Data/ or /Lua/ relative to the .exe
	resourcesPath.append("/../Demon Barber Tycoon");
#endif

	luaPath = resourcesPath + "/Lua/";
	
	// Data path
	dataPath = resourcesPath + "/Data/";
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

unsigned int W::randUpTo(int x) {
	return twister()%x;
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
