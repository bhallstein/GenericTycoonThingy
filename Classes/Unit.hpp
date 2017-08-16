/* 
 * Unit.hpp
 *
 */

#ifndef UNIT_H
#define UNIT_H

#include <iostream>
#include <map>

#include "types.hpp"
#include "PlaceableManager.hpp"
#include "BehaviourParticipant.hpp"

class NavNode;
class Building;
class Level;
class Furnishing;

struct unitInfo {
	W::Colour::T col, hoverCol;
	std::vector<std::string> compatibleBehaviours;
	bool isStaff;
	int hireCost;
};

namespace W { namespace EventType {
	extern T INTERRUPT_UNITPICKUP;
} }

class Unit : public PlaceableManager, public BehaviourParticipant {
public:
	Unit(W::EventHandler *, W::NavMap *, const char *_type, Level *, bool _placeableMode);
	~Unit();
	
	// Properties
	std::string type;
	int prev_x, prev_y;
	int dest_x, dest_y;
	Building *dest_building;
	
	// Methods
	void receiveEvent(W::Event *);
	void update();
	bool canPlace(int _x, int _y);
	void finalizePlacement();
	W::Colour::T col();
	
	// Utility methods
	void getDespawnPoint(int *x, int *y);
	void destroy();
	bool voyage(int _x, int _y);
	bool arrived;
	void runAnimation(/* Animation...? */);
	bool animation_finished;
	
	Building* getContextBuilding() { return contextBuilding; }
	
	static bool initialize(); 	// Populate static unitTypes from units.lua
	static bool initialized;
	
	std::string nextBehaviour;

	static int hireCostForType(const char *);

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
	
	void pickUp();
	
	bool incrementLocation();		// Move along route. Returns false if an obstacle is encountered.
	inline bool atDest();
	inline bool inHinterland();
	
	// Properties
	W::Colour::T u_colour;
	W::Colour::T u_hoverColour;
	std::vector<std::string> *u_compatibleBehaviours;
	std::vector<std::string>* getCompatibleBehaviours();
	bool u_isStaff;
	int u_hireCost;
	
	bool hired;
	
	W::NavMap *navmap;
	std::vector<W::position> route;
	Level *level;
	Building *contextBuilding;
	
	// Static members
	static std::map<std::string, unitInfo> unitTypes;	// e.g. "civilian" => struct unitInfo { }
	static W::Colour::T defaultColour;
	static W::Colour::T defaultHoverColour;
	
	void printDebugInfo();
};

#endif
