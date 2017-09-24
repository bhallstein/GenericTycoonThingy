/*
 * Generic Tycoon Thingy
 *
 * =============
 *  types.hpp
 * =============
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

/*
 * Types.hpp – useful declarations & junk
 *
 */

#ifndef TYPES_H
#define TYPES_H

#include <string>
#include "W.h"
#include "Lua.hpp"

#ifndef DBT_MAX_PATH
#define DBT_MAX_PATH 200
#endif

#define HINTERLAND_WIDTH 2

namespace Direction {
	enum T { LEFTWARD, RIGHTWARD, UPWARD, DOWNWARD };
}

namespace UnitMode {
	enum T { IDLE, VOYAGING, ANIMATING };
}

namespace SeekTarget {
	enum Type {
		Pie,
		Haircut,
		
		__N
	};
	extern std::map<Type, std::string> Targets;
};

typedef struct door {
	W::position pos;
	Direction::T orientation;
} door;

bool streq(const char *a, const char *b);
bool strstarts(const char *str, const char *search);
W::Colour strToColour(const char *s);

// Useful lua fle loading/executing fn
bool luaLoad(const std::string &filename, lua_State **);

#endif
