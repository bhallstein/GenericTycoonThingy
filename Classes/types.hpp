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
