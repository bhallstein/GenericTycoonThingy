/*
 * Generic Tycoon Thingy
 *
 * ==========
 *  TLO.cpp
 * ==========
 *
 * Copyright (C) 2012 - Ben Hallstein
 * All rights reserved
 *
 */

#include "TLO.hpp"
#include "Serializer.hpp"

Serializable::serialization_descriptor TLO::sd;

void TLO::initialize() {
	TLO::sd["uid"] = makeSerializer(&TLO::uid);
	TLO::sd["rct"] = makeSerializer(&TLO::rct);
	TLO::sd["type"] = makeSerializer(&TLO::type);
	TLO::sd["destroyed"] = makeSerializer(&TLO::destroyed);
  // Should `destroyed` ever be serialized as true?!
}
