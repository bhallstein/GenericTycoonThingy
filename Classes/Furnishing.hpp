/*
 * Furnishing.hpp
 * 
 */

#ifndef FURNITURE_H
#define FURNITURE_H

#include <iostream>

#include "types.hpp"
#include "PlaceableManager.hpp"
#include "BehaviourParticipant.hpp"

class ResponderMap;
class Building;
class Unit;

struct furnishingInfo {
	W::Colour col, hoverCol;
	std::vector<W::rect> plan;
	std::vector<std::string> compatibleBehaviours;
	std::map<std::string, W::position> interactionPoints;
	int cost;
};

class Furnishing : public PlaceableManager, public BehaviourParticipant {
public:
	Furnishing(W::NavMap *, const char *_type, Level *, Building *, bool _placeableMode);
	~Furnishing();
	
	// Properties
	std::string type;
	
	// Methods
	void receiveEvent(W::Event *);
	void update();
	virtual bool canPlace(int _x, int _y);
	virtual void finalizePlacement();
	W::Colour& col();
	
	void getInteractionPoint(const char *_unitType, W::position &);
		// Throws exception if no IP exists for the given unit type
	bool requiresStaff(const char *uType);
	void runAnimation(int duration);
	bool animationFinished;
	
	static bool initialize();	// Populate our static furnishingTypes map from furnishing.lua
	static bool initialized;

	static int costForType(const char *);
protected:
	struct furnishingInfo *fInfo;
	std::vector<std::string>* getCompatibleBehaviours();
	
	W::NavMap *navmap;
	Building *contextBuilding;
	int animFrames, animationDuration;

	Level *level;
	bool purchased;
	
	// Static members
	static std::map<std::string, struct furnishingInfo> furnishingTypes;	// e.g. "barber's chair" => struct unitInfo { }
	static W::Colour defaultColour;
	static W::Colour defaultHoverColour;
};


class Door : public Furnishing {
public:
	Door(W::NavMap *, const char *_type, Level *, bool _placeableMode);
	~Door();
	
	bool canPlace(int _x, int _y);
	void finalizePlacement();
};

#endif
