#include "types.hpp"

inline bool streq(const char *a, const char *b) {
	return !strcmp(a, b);
}

W::Colour::T strToColour(const char *s) {
	W::Colour::T c;
	using namespace W::Colour;
	if      (streq(s, "black" )) c = BLACK;
	else if (streq(s, "white" )) c = WHITE;
	else if (streq(s, "red"   )) c = RED;
	else if (streq(s, "green" )) c = GREEN;
	else if (streq(s, "blue"  )) c = BLUE;
	else if (streq(s, "yellow")) c = YELLOW;
	else if (streq(s, "purple")) c = PURPLE;
	return c;
}

bool strstarts(const char *str, const char *search) {
	for (int i=0; str[i] != '\0' && search[i] != '\0'; i++)
		if (str[i] != search[i]) return false;
	return true;
}
