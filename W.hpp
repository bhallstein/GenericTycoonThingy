/*
 * W.hpp - Cross-platform OpenGL Bobulator
 *
 */

#ifndef THEW_H
#define THEW_H

#include <fstream>
#include <string>
#include <vector>

#include "Classes/types.hpp"
#include "WindowManager.hpp"

class View;
class Event;

class W {
public:
	W(WindowManager *);
	~W();
	
	// Methods
	
	// Window shizzle
	bool goWindowed();
	bool goFullscreen();
	bool setResolution(int _x, int _y);
	void setWindowTitle(const char *);
	
	// Drawing shizzle
	void setUpOpenGL();
	void startDrawing();
	void finishDrawing();
	void setUpDrawingForView(View *);	// Called by View::_draw(), to prepare the current view
	void drawRect(float _x, float _y, float _width, float _height, colour);

	// Event shizzle
	void addEvent(Event &);
	std::vector<Event>* getEvents();
	void clearEvents();
	
	// File shizzle
	//FILE* filePointerToResource(std::string);	// Be sure to call fclose() when you're done
	void initializePaths();
	bool isValidDir(const char *);
	bool createDir(const char *);

	// Stringy output
	void warning(const char *msg, const char *title = "Warning");
	void log(const char *);

	//void* getView();
	int width();
	int height();

	// Properties
	std::string settingsPath;
	std::string resourcesPath;
	std::string logfilePath;
	std::ofstream logfile;
	
protected:
	// Methods
	//void setBackBufferSize(int _x, int _y);
	
	// Properties
	//enum mode_type { WINDOWED, FULLSCREEN, NONE } mode;
	WindowManager *winManager;
	bool opengl_needs_setting_up;
	View *current_drawn_view;

	std::vector<Event> events;
};

#endif
