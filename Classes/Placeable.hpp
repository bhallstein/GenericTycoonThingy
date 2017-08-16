/*
 * Placeable.hpp - sort of an abstract placement manager for MappedObj
 *
 */

#ifndef PLACEABLE_H
#define PLACEABLE_H

#include <iostream>
#include <map>

#include "types.hpp"
#include "EventResponder.hpp"
#include "ResponderMap.hpp"

class MappedObj;

class Placeable : public EventResponder {
public:
	Placeable(MappedObj *, ResponderMap *);
	~Placeable();
	void receiveEvent(Event *ev);
	MappedObj *mo;
	ResponderMap *rm;
};

#endif
