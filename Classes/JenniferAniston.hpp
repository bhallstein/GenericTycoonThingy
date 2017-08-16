/*
 * JenniferAniston.hpp - JenniferAniston manages flexible view positioning
 * 
 * JenniferAniston manages the positioning of a view. A View may query JenniferAniston to obtain its position.
 * 
 * - A view must be pinned at one of its four corners. Specify which in the `corner` parameter.
 * - The corner’s coords can be either fixed (pixels) or proportional, as can the width and height.
 * 		- Specify which in the two pos_type parameters.
 * 		- If you use proportional positioning, the width and height will be between 0 and 1
 * 
 * In the future, JenniferAniston may be able to read from layout files, and scale positions for different resolutions.
 *
 */

#ifndef JENNIFERANISTON_H
#define JENNIFERANISTON_H

#include <iostream>

#include "../W.hpp"

enum corner { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };
enum pos_type { PFIXED, PPROPORTIONAL };

class JenniferAniston {
public:
	JenniferAniston(W *_theW, corner _corner, pos_type _pos_method, pos_type _size_method, float _x, float _y, float _w, float _h) :
		theW(_theW), fixed_corner(_corner),
		pos_method(_pos_method), size_method(_size_method),
		corner_x(_x), corner_y(_y), w(_w), h(_h)
	{
		
	}
	
	// Methods
	void refresh() {
		int Ww = theW->width();
		int Wh = theW->height();
		
		if (size_method == PFIXED)
			width = w, height = h;
		else
			width = w * Ww, height = h * Wh;
		
		// Set x
		if (fixed_corner == TOP_LEFT || fixed_corner == BOTTOM_LEFT)
			x = (pos_method == PFIXED ? corner_x : corner_x * Ww);
		else {
			int rval = Ww - (pos_method == PFIXED ? corner_x : corner_x * Ww);
			x = rval - width;
		}
		// Set y
		if (fixed_corner == TOP_LEFT || fixed_corner == TOP_RIGHT)
			y = (pos_method == PFIXED ? corner_y : corner_y * Wh);
		else {
			int bval = Wh - (pos_method == PFIXED ? corner_y : corner_y * Wh);
			y = bval - height;
		}
	}
	
	void setCorner(int _x, int _y) {
		corner_x = _x, corner_y = _y;
	}
	
	// Properties
	int x, y, width, height;
	
private:
	// Properties
	W *theW;
	corner fixed_corner;
	pos_type pos_method, size_method;
	float corner_x, corner_y, w, h;
	
};

#endif
