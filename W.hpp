/*
 * W.hpp - Cross-platform OpenGL Bobulator
 *
 */

#ifndef THEW_H
#define THEW_H

#include <string>
#include <vector>

#include "Classes/types.h"
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
	void frameChanged();
	void startDrawing();
	void finishDrawing();
	void setUpDrawingForView(View *);	// Called by View::_draw(), to prepare the current view
	void drawRect(float _x, float _y, float _width, float _height, colour_name);

	// Event shizzle
	void addEvent(Event &);
	std::vector<Event>* getEvents();
	void clearEvents();
	
	// File shizzle
	//FILE* filePointerToResource(std::string);	// Be sure to call fclose() when you're done
	std::string pathForResource(std::string);
	std::string pathToSettingsDir();

	//void* getView();
	int width();
	int height();
	
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
