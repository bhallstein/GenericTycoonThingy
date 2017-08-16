/*
 * LevelMap.hpp
 *
 */

#ifndef __DBT__LevelMap__
#define __DBT__LevelMap__

#include <string>
#include <vector>

#include "W.h"
#include "LuaObj.h"

class LevelState;
class LevelView;
class TLO;
class Unit;
class Furnishing;
class SpawnPoint;

class LevelMap {
public:
	LevelMap(LevelState *, LevelView *);
	~LevelMap();
	
	void update();
	bool load(LuaObj &mapObj);
	std::string save();
	
	W::EventPropagation::T keyEvent(W::Event *);
	
	int width() { return mapSize.width; }
	int height() { return mapSize.height; }
	
private:
	LevelState *levelState;
	LevelView *levelView;
	
	W::NavMap *navmap;
	
//	std::vector<Unit*> customers;
	typedef std::vector<TLO*> tloVec;
	tloVec units;
	tloVec furnishings;
	tloVec spawnPoints;
	
	Furnishing* createFurnishing(bool placeableMode, const std::string &type, const W::position &pos = W::position());
	Furnishing* createFurnishing(LuaObj &);
	Unit* createUnit(bool placeableMode, const std::string &type, const W::position &pos = W::position());
	Unit* createUnit(LuaObj &);
	SpawnPoint* createSpawnPoint(bool placeableMode, const W::position &pos = W::position());
	SpawnPoint* createSpawnPoint(LuaObj &);
	void destroyTLO(TLO *);
	
	void updateTLOVec(tloVec &);
		// Calls update() on each TLO.
		//  - After update(), if the TLO is 'destroyed', destroy it.
		//  - It is the dying TLO's responsibility to inform any subscribers
		//    of its demise.
	
	// Map properties & loading
	bool loaded;
	W::size mapSize;
};

#endif
