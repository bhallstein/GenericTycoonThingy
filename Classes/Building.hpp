/*
 * Building.hpp
 *
 */

#ifndef BUILDING_H
#define BUILDING_H

#include <iostream>
#include <map>
#include <string>

#include "types.hpp"
#include "TLO.hpp"

class Level;
class LuaHelper;
class Unit;
class Furnishing;

struct buildingInfo {
	W::Colour col, hoverCol;
	std::vector<std::string> allowedFurnishings;
};

class Building : public TLO {
public:
	Building(W::EventHandler *, W::NavMap *, const char *_type, std::vector<W::rect> *_plan, W::position &_pos, Level *);
	~Building();
	
	// Properties
	std::string type;
	W::Colour b_colour;
	W::Colour b_hoverColour;
	std::vector<std::string> *b_allowedFurnishings;
	
	// Methods
	void receiveEvent(W::Event *);		// Handle mouse events
	void update();
	W::Colour& col();
	
//	void getEntryPoint(int *_x, int *_y);
	void getQueuePoint(int *_x, int *_y);
	void addToQueue(Unit *);
	void addFurnishing(Furnishing *);
	void removeFurnishing(Furnishing *);
	void addStaff(Unit *);
	void removeStaff(Unit *);
	
	bool objIsEntirelyInsideBuilding(W::MappedObj *);
	
	static bool initialize();	// Populate our static buildingTypes map from buildings.lua
	static bool initialized;
	
protected:
	// Properties
	bool clicked;
	bool hover;
	int time_hovered;
	W::NavMap *navmap;
	Level *level;
	// Dispatchery
	std::vector<Unit*> Q;
	std::vector<Unit*> staff;
	std::vector<Furnishing*> furnishings;
	std::map<Furnishing*, Unit*> staffBindings;
	
	// Info on building types, saved as static members for private use by Building & its instances.
	static std::map<std::string, struct buildingInfo> buildingTypes;	// e.g. "pieshop" => struct buildingInfo { }
	static W::Colour defaultColour;
	static W::Colour defaultHoverColour;
	
	static std::vector<std::map<Furnishing*, Unit*>::iterator> _ind_array;
};

#endif
