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
	W::Colour col, hoverCol;
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
	W::position prev_pos;
	W::position dest;
	Building *dest_building;
	
	ShopKeeperBehaviour *skBehaviour;	// Only used by shopkeepers
	
	// Methods
	void receiveEvent(W::Event *);
	void update();
	bool canPlace(int _x, int _y);
	void finalizePlacement();
	W::Colour& col();
	
	// Utility methods
	void getDespawnPoint(W::position &);
	void destroy();
	bool voyage(const W::position &_dest);
	bool arrived;
	void runAnimation(/* Animation...? */);
	bool animation_finished;
	
//	Building* getContextBuilding() { return contextBuilding; }
	
	static bool initialize(); 	// Populate static unitTypes from units.lua
	static bool initialized;

	static unitInfo * infoForType(const char *);

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
	struct unitInfo *uInfo;
	std::vector<std::string>* getCompatibleBehaviours();
	
	bool hired;
	
	W::NavMap *navmap;
	std::vector<W::position> route;
	Level *level;
//	Building *contextBuilding;
	
	// Static members
	static std::map<std::string, unitInfo> unitTypes;
	static W::Colour defaultColour;
	static W::Colour defaultHoverColour;
	
	void printDebugInfo();
};

#endif
