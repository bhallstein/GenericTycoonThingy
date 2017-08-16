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
	TLO(W::EventHandler *);
	~TLO();
	
	virtual void update() { }		// Default implementations of these are provided, so that
	virtual W::Colour::T col() {	// implementation of fns is optional for derived classes
		return W::Colour::WHITE;
	}
	
	bool destroyed;
	bool hover;
	float rotation;
	W::EventHandler *eh;
};

#endif
