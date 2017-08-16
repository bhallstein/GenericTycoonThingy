/*
 * TLO.hpp - Top Level Object class
 *
 * For useful top-level-object related functionality
 *
 */

#ifndef TLO_H
#define TLO_H

#include "W.h"

class TLO : public W::MappedObj {
public:
	TLO();
	~TLO();
	
	//virtual void update() { }		// Default implementations of these fns are provided so that
	virtual W::Colour& col() {		// they're optional for derived classes
		return W::Colour::White;
	}
	
	bool destroyed;
	bool hover;
	float rotation;
	W::EventHandler *eh;
};

#endif
