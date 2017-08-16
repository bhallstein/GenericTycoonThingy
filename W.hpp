/*
 * W.hpp - a cross-platform OpenGL combobulator
 *
 */

#ifndef THEW_H
#define THEW_H

#include <string>
#include <vector>

#include "Classes/types.h"

class View;
class Event;

struct NativeObjs;

class W {
public:
	W();
	~W();
	
	// Methods
	
	// Window setup
	bool goWindowed();
	bool goFullscreen();
	
	void setWindowTitle(std::string);
	void setUpOpenGL();
	void frameChanged();
	void* getView();
	
	int width();
	int height();
	
	// Events
	std::vector<Event *>* getEvents();
	void sendQuitEvent();
	
	// Drawing
	void startDrawing();		// Called by the app delegate
	void finishDrawing();		// 
	void setUpDrawingForView(View *);	// Called by View::_draw(), to prepare the current view
	void drawRect(float _x, float _y, float _width, float _height, colour_name);
	
	// File access
	//FILE* filePointerToResource(std::string);	// Be sure to call fclose() when you're done
	std::string pathForResource(std::string);
	
protected:
	// Methods
	void setBackBufferSize(int _x, int _y);
	void closeWindow();
	
	// Properties
	struct NativeObjs *objs;
	enum mode_type { WINDOWED, FULLSCREEN, NONE } mode;
	bool opengl_needs_setting_up;
	View *current_drawn_view;

	std::vector<Event *> events;

	bool quit_event;
	int prev_width, prev_height;
	
	bool a_lion_is_here;
};

#endif
