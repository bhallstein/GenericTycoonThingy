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
	W::Colour::T col, hoverCol;
	std::vector<W::rect> plan;
	std::vector<std::string> compatibleBehaviours;
	std::map<std::string, W::position> interactionPoints;
	int cost;
};

class Furnishing : public PlaceableManager, public BehaviourParticipant {
public:
	Furnishing(W::EventHandler *, W::NavMap *, const char *_type, Level *, Building *, bool _placeableMode);
	~Furnishing();
	
	// Properties
	std::string type;
	
	// Methods
	void receiveEvent(W::Event *);
	void update();
	virtual bool canPlace(int _x, int _y);
	virtual void finalizePlacement();
	W::Colour::T col();
	
	void getInteractionPoint(const char *_unitType, int *_x, int *_y);
	bool requiresStaff(const char *uType);
	void runAnimation(int duration);
	bool animationFinished;
	
	static bool initialize();	// Populate our static furnishingTypes map from furnishing.lua
	static bool initialized;

	static int costForType(const char *);
protected:
	W::Colour::T f_colour;
	W::Colour::T f_hoverColour;
	std::vector<std::string> *f_compatibleBehaviours;
	std::map<std::string, W::position> *f_interactionPoints;
	std::vector<std::string>* getCompatibleBehaviours();
	
	W::NavMap *navmap;
	Building *contextBuilding;
	int animFrames, animationDuration;

	Level *level;
	bool purchased;
	
	// Static members
	static std::map<std::string, struct furnishingInfo> furnishingTypes;	// e.g. "barber's chair" => struct unitInfo { }
	static W::Colour::T defaultColour;
	static W::Colour::T defaultHoverColour;
};


class Door : public Furnishing {
public:
	Door(W::EventHandler *, W::NavMap *, const char *_type, Level *, bool _placeableMode);
	~Door();
	
	bool canPlace(int _x, int _y);
	void finalizePlacement();
};

#endif
