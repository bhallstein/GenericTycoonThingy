/*
 * TLO.hpp
 *
 * For useful top-level-object related functionality
 *
 * About auto-serialization
 *  - subclass creates a serialization_descriptor, which contains Serializers which can read/write
 *    the subclass's properties to/from lua
 *  - this initilization can be done in the existing static initialize() method
 *  - subclass supplies descriptor to TLO via the getSD() virtual method
 *  - TLO can then use the descriptor to serialize & deserialize objects
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
	TLO(LevelState *_ls, LevelMap *_lm, LevelView *_lv, W::NavMap *_nm) :
		levelState(_ls),
		levelMap(_lm),
		levelView(_lv),
		navmap(_nm),
		type(NO_TYPE),
		destroyed(false)
	{
		// hai tlo
	}
	virtual ~TLO() { }
	
	void setType(const std::string &_type) { type = _type; }
	virtual void setUp() = 0;
	virtual void update() = 0;
	
	bool destroyed;
	void destroy() { destroyed = true; }
	
	static bool initialize() {
		TLO::sd["uid"] = makeSerializer(&TLO::uid);
		TLO::sd["rct"] = makeSerializer(&TLO::rct);
		TLO::sd["type"] = makeSerializer(&TLO::type);
		TLO::sd["destroyed"] = makeSerializer(&TLO::destroyed);
			// Should `destroyed` ever be serialized as true?!
		return true;
	}
	
	UID uid;
	W::rect rct;
	
protected:
	sdvec getSDs() {
		sdvec vec, vec2 = _getSDs();
		vec.push_back(&TLO::sd);
		vec.insert(vec.end(), vec2.begin(), vec2.end());
		return vec;
	}
	virtual sdvec _getSDs() { return sdvec(); }
	
	LevelState *levelState;
	LevelMap   *levelMap;
	LevelView  *levelView;
	W::NavMap *navmap;
	
	std::string type;
	
private:
	static serialization_descriptor sd;

};

#endif
