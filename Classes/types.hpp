/*
 * Types.hpp – useful declarations 'n junk
 *
 */

#ifndef TYPES_H
#define TYPES_H

#include <string>

#ifndef MAX_PATH
#define MAX_PATH 200
#endif

#define INFINITAH 9999999

// Game stages
#define TUTORIAL_STAGE 2649
#define DEMON_STAGE 2651

// Coordinates

typedef struct intcoord {
	int x, y;
} intcoord;
typedef struct floatcoord {
	float a, b;
} floatcoord;

#define LEFTWARD  0
#define UPWARD    1
#define RIGHTWARD 2
#define DOWNWARD  3

typedef int direction;
typedef struct door {
	intcoord coord;
	direction orientation;
} door;

enum utility_colours { _BLACK_, _WHITE_, _RED_, _GREEN_, _BLUE_, _YELLOW_, _PURPLE_ };
typedef struct colour {
	colour() { };
	colour(utility_colours c) {
		if (c == _BLACK_) red = green = blue = 0, alpha = 0.5;
		else if (c == _WHITE_)  red = green = blue = alpha = 1;
		else if (c == _RED_)    red = 1, green = blue = 0, alpha = 1;
		else if (c == _GREEN_)  red = 0, green = 1, blue = 0, alpha = 1;
		else if (c == _BLUE_)   red = green = 0, blue = 1, alpha = 1;
		else if (c == _YELLOW_) red = green = 1, blue = 0, alpha = 1;
		else if (c == _PURPLE_) red = 0.47, green = 0, blue = 0.78, alpha = 1;
	}
	colour(std::string s) {
		if (s == "black") red = green = blue = 0, alpha = 0.5;
		else if (s == "white")  red = green = blue = alpha = 1;
		else if (s == "red")    red = 1, green = blue = 0, alpha = 1;
		else if (s == "green")  red = 0, green = 1, blue = 0, alpha = 1;
		else if (s == "blue")   red = green = 0, blue = 1, alpha = 1;
		else if (s == "yellow") red = green = 1, blue = 0, alpha = 1;
		else if (s == "purple") red = 0.47, green = 0, blue = 0.78, alpha = 1;
		colname = s;
	}
	float red, green, blue, alpha;
	std::string colname;
} colour;

class MsgException : public std::exception {
public:
	MsgException(const char *s) {
		msg = s;
	}
	~MsgException() throw() { }
	virtual const char* what() {
		return "Message exception";
	}
	std::string msg;
};

#endif
