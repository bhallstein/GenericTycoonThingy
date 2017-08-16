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

#define INFINITAH 99999999

// Game stages
#define TUTORIAL_STAGE 2649
#define DEMON_STAGE 2651

// Hinterland
#define HINTERLAND_WIDTH 2

// Coordinates
typedef struct intcoord {
	int x, y;
} intcoord;
typedef struct floatcoord {
	float a, b;
} floatcoord;

namespace Direction {
	enum Enum { LEFTWARD, RIGHTWARD, UPWARD, DOWNWARD };
};

typedef struct door {
	intcoord coord;
	Direction::Enum orientation;
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
