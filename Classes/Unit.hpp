/* 
 * Unit.hpp
 *
 */

#ifndef UNIT_H
#define UNIT_H

#include <iostream>
#include <map>

#include "types.hpp"
#include "MappedObj.hpp"
#include "BehaviourParticipant.hpp"
#include "../W.hpp"

class NavMap;
class NavNode;
class Building;
class Level;
class Furnishing;

struct unitInfo {
	std::string col, hoverCol;
	std::vector<std::string> compatibleBehaviours;
	bool isStaff;
	int hireCost;
};

class Unit : public MappedObj, public BehaviourParticipant {
public:
	Unit(ResponderMap *, NavMap *, const char *_type, Level *, bool _placeableMode);
	~Unit();
	
	// Properties
	std::string type;
	int prev_x, prev_y;
	int dest_x, dest_y;
	Building *dest_building;
	Level *level;
	
	// Methods
	void receiveEvent(Event *);
	void update();
	bool canPlace(int _x, int _y);
	void finalizePlacement();
	const char * col();
	
	// Utility methods
	void getDespawnPoint(int *x, int *y);
	void destroy();
	bool voyage(int _x, int _y);
	bool arrived;
	void runAnimation(/* Animation...? */);
	bool animation_finished;
	
	static bool initialize(W *); 	// Populate static unitTypes from units.lua
	static bool initialized;
	
	std::string nextBehaviour;

	// UnitInfo gets
	static int getUnitHireCost(std::string); //lookup a unitInfo hireCost from unitTypes
protected:
	enum Mode {
		WAITING,	// Waiting always precedes voyaging?
		VOYAGING,
		ANIMATING,
		IDLE
	} mode;

	void wait();
	int frames_waited;
	int frames_animated;
	void incrementAnimation();
	
	bool incrementLocation();		// Move along route. Returns false if an obstacle is encountered.
	inline bool atDest();
	inline bool inHinterland();
	
	// Properties
	std::string *u_colour;
	std::string *u_hoverColour;
	std::vector<std::string> *u_compatibleBehaviours;
	std::vector<std::string>* getCompatibleBehaviours();
	bool u_isStaff;
	int u_hireCost;
	
	NavMap *navmap;
	std::vector<NavNode *> route;
	bool hover;
	
	// Static members
	static std::map<std::string, unitInfo> unitTypes;	// e.g. "civilian" => struct unitInfo { }
	static std::string defaultColour;
	static std::string defaultHoverColour;
};

#endif
