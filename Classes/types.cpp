/*
 * Generic Tycoon Thingy
 *
 * =============
 *  types.cpp
 * =============
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#include "types.hpp"

bool streq(const char *a, const char *b) {
	return !strcmp(a, b);
}

bool strstarts(const char *str, const char *search) {
	for (int i=0; str[i] != '\0' && search[i] != '\0'; i++)
		if (str[i] != search[i]) return false;
	return true;
}

W::Colour strToColour(const char *s) {
	if      (streq(s, "black" )) return W::Colour::Black;
	else if (streq(s, "white" )) return W::Colour::White;
	else if (streq(s, "red"   )) return W::Colour::Red;
	else if (streq(s, "green" )) return W::Colour::Green;
	else if (streq(s, "blue"  )) return W::Colour::Blue;
	else if (streq(s, "yellow")) return W::Colour::Yellow;
	else if (streq(s, "purple")) return W::Colour::Purple;
	else if (streq(s, "transparent black")) return W::Colour::TransparentBlack;
	else return W::Colour::Black;
}

bool luaLoad(const std::string &filename, lua_State **L) {
	// Load file
	*L = luaL_newstate();
	bool loadError = luaL_loadfile(*L, filename.c_str());
	if (loadError) {
		W::log
			<< "couldn't open lua file '" << filename << "' - error was: "
			<< lua_tostring(*L, -1) << std::endl;
		lua_close(*L);
		return false;
	}
	bool runError = lua_pcall(*L, 0, 0, 0);
	if (runError) {
		W::log
			<< "couldn't execute file '" << filename << "' - error was: "
			<< lua_tostring(*L, -1) << std::endl;
		lua_close(*L);
		return false;
	}
	return true;
}
