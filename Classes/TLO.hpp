/*
 * Generic Tycoon Thingy
 *
 * ===========
 *  TLO.hpp
 * ===========
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

/*
 * TLO.hpp - contains useful top-level-object related functionality
 *
 */

#ifndef TLO_H
#define TLO_H

#include "W.h"
#include "Serializer.hpp"
#include "Serializable.hpp"

#define NO_TYPE "no_type"
	// When a typed TLO is constructed, its type is not yet known

class LevelState;
class LevelMap;
class LevelView;

class TLO : public Serializable {
public:
	TLO(LevelMap *_lm, LevelView *_lv, W::NavMap *_nm) :
		levelMap(_lm),
		levelView(_lv),
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
	
	static void initialize() {
		TLO::sd["uid"] = makeSerializer(&TLO::uid);
		TLO::sd["rct"] = makeSerializer(&TLO::rct);
		TLO::sd["type"] = makeSerializer(&TLO::type);
		TLO::sd["destroyed"] = makeSerializer(&TLO::destroyed);
			// Should `destroyed` ever be serialized as true?!
	}
	
	UID uid;
	W::rect rct;
	
	std::string type;
	
protected:
	virtual void getSDs(sdvec &vec) {
		vec.push_back(&TLO::sd);
	}
	
	LevelMap   *levelMap;
	LevelView  *levelView;
	W::NavMap *navmap;
	
private:
	static serialization_descriptor sd;

};

#endif
