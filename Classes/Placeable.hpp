/*
 * Placeable.h
 *
 */

#ifndef PLACEABLE_H
#define PLACEABLE_H

#include <iostream>

#include "types.h"
#include "MappedObj.hpp"
#include "NavMap.hpp"
#include "View.hpp"


class Placeable : public MappedObj
{
public:
	Placeable(NavMap *, View *);
	~Placeable();

	// Methods
	void receiveEvent(Event *ev);
	sf::Color col();

	// Properties
	bool destroyed;

protected:
	// Properties
	enum mode_types { PLACEMENT, PLACED } mode;
	bool clicked;
	NavMap *navmap;
	View *view;
	
};

#endif
