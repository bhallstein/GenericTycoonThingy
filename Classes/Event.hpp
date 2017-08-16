/*
 * Event.hpp
 *
 */

#ifndef EVENTEROONY_H	// EVENT_H seems to be reserved on OS X
#define EVENTEROONY_H

class Event {
public:
	Event() { }
	~Event() { }
	
	// Properties
	enum event_type {
		KEYPRESS,
		MOUSEMOVE, LEFTCLICK, RIGHTCLICK,
		SCREENEDGE_TOP, SCREENEDGE_BOTTOM, SCREENEDGE_LEFT, SCREENEDGE_RIGHT,
		CLOSED,
		UNKNOWN_EVENT
	} type;
	enum key_code {
		K_A,	K_B,	K_C,	K_D,
		K_E,	K_F,	K_G,	K_H,
		K_I,	K_J,	K_K,	K_L,
		K_M,	K_N,	K_O,	K_P,
		K_Q,	K_R,	K_S,	K_T,
		K_U,	K_V,	K_W,	K_X,
		K_Y,	K_Z,
		
		K_0,	K_1,	K_2,
		K_3,	K_4,	K_5,
	    K_6,	K_7,	K_8,	K_9,
		
		K_LEFT_ARROW,	K_RIGHT_ARROW,
		K_UP_ARROW,		K_DOWN_ARROW,
		
		K_ESC,	K_RETURN,
		
		K_OTHER
	} key;
	int x, y;		// Block coords
	float a, b;		// Float coords (offset)
	
	// Methods
	void setCoords(int _x, int _y, int _a = 0, int _b = 0) {
		x = _x, y = _y;
		a = _a, b = _b;
	}
	void setType(event_type _type) { type = _type; }
	void setKey(key_code _key) { key = _key; }
	
};

#endif
