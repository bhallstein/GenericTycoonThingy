/*
 * Furnishing.hpp
 * 
 */

#ifndef FURNITURE_H
#define FURNITURE_H

#include <iostream>

#include "types.hpp"
#include "MappedObj.hpp"
#include "BehaviourParticipant.hpp"

class ResponderMap;
class NavMap;
class W;
class Building;
class Unit;

struct furnishingInfo {
	std::string col, hoverCol;
	std::vector<intcoord> groundplan;
	std::vector<std::string> compatibleBehaviours;
	std::map<std::string, intcoord> interactionPoints;
	int cost;
};

class Furnishing : public MappedObj, public BehaviourParticipant {
public:
	Furnishing(ResponderMap *, NavMap *, const char *_type, Building *);
	~Furnishing();
	
	// Properties
	std::string type;
	
	// Methods
	void receiveEvent(Event *);
	void update();
	bool canPlace(int _x, int _y);
	void finalizePlacement();
	const char * col();
	
	void getInteractionPoint(const char *_unitType, int *_x, int *_y);
	bool requiresStaff(const char *uType);
	void runAnimation(int duration);
	bool animationFinished;
	
	static bool initialize(W *);	// Populate our static furnishingTypes map from furnishing.lua
	static bool initialized;

	// UnitInfo gets
	static int getFurnishingCost(std::string); //lookup a FurnishingInfo cost from furnishingTypes
protected:
	std::string *f_colour;
	std::string *f_hoverColour;
	std::vector<std::string> *f_compatibleBehaviours;
	std::map<std::string, intcoord> *f_interactionPoints;
	std::vector<std::string>* getCompatibleBehaviours();
	
	NavMap *navmap;
	Building *contextBuilding;
	int animFrames, animationDuration;
	
	// Static members
	static std::map<std::string, struct furnishingInfo> furnishingTypes;	// e.g. "barber's chair" => struct unitInfo { }
	static std::string defaultColour;
	static std::string defaultHoverColour;
};

#endif
