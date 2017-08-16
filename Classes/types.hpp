/*
 * Types.hpp – useful declarations 'n junk
 *
 */

#ifndef TYPES_H
#define TYPES_H

#include <string>
#include "W.h"

#ifndef DBT_MAX_PATH
#define DBT_MAX_PATH 200
#endif

#define HINTERLAND_WIDTH 2

namespace Direction {
	enum T { LEFTWARD, RIGHTWARD, UPWARD, DOWNWARD };
};

typedef struct door {
	W::position pos;
	Direction::T orientation;
} door;

inline bool streq(const char *a, const char *b);
W::Colour::T strToColour(const char *s);
bool strstarts(const char *str, const char *search);

#endif
