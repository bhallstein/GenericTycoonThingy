/*
 * Generic Tycoon Thingy
 *
 * ==========
 *  TLO.hpp
 * ==========
 *
 * Copyright (C) 2012 - Ben Hallstein
 * All rights reserved
 *
 */

#ifndef TLO_H
#define TLO_H

#include "W.h"
#include "UIDManager.hpp"
#include "Serializable.hpp"

#define NO_TYPE "no_type"
  // When a typed TLO is constructed, its type is not yet known

class LevelMap;
class View__Game;

class TLO : public Serializable {
public:
	TLO(LevelMap *_lm, View__Game *_lv, W::NavMap *_nm) :
		levelMap(_lm),
		view__game(_lv),
		navmap(_nm),
		type(NO_TYPE),
		destroyed(false)
	{
		// hai tlo
	}
	virtual ~TLO()
	{
		UIDManager::unregisterTLO(this);
	}
	
	void setType(const std::string &_type) { type = _type; }
	void setUp() {
		if (!deserialized) uid = UIDManager::getNewUID();
		UIDManager::registerTLO(this);
		_setUp();
	}
	virtual void _setUp() = 0;
	virtual void update() = 0;
	
	bool destroyed;
	void destroy() { destroyed = true; }
	
	static void initialize();
	
	UID uid;
	W::rect rct;
	
	std::string type;
	
protected:
	virtual void getSDs(sdvec &vec) {
		vec.push_back(&TLO::sd);
	}
	
	LevelMap   *levelMap;
	View__Game  *view__game;
	W::NavMap *navmap;
	
private:
	static serialization_descriptor sd;

};

#endif
