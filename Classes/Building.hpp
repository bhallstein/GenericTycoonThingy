/*
 * Building.hpp
 *
 */

#ifndef BUILDING_H
#define BUILDING_H

#include <iostream>
#include <map>
#include <string>
#include <queue>

#include "types.hpp"
#include "TLO.hpp"

class Level;
class LuaHelper;
class Unit;
class Furnishing;
class Behaviour;
class CustomerBehaviour;
class DispatchingBehaviour;
class ShopKeeperBehaviour;

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
	struct buildingInfo *bInfo;
	
	// Methods
	void receiveEvent(W::Event *);		// Handle mouse events
	void update();
	W::Colour& col();
	
	void getQueuePoint(W::position &);
	void queueCustomer(CustomerBehaviour *);
	void customerEntering();	// Behaviours must call these so building can track how
	void customerLeaving();		// many customers are inside it
//	void getEntryPoint(int *_x, int *_y);

	void addFurnishing(Furnishing *);
	void removeFurnishing(Furnishing *);
	Furnishing* getRandomAvailableFurnishing();

	void addShopKeeper(ShopKeeperBehaviour *);
	void removeShopKeeper(ShopKeeperBehaviour *);
	ShopKeeperBehaviour* getRandomAvailableShopKeeperBehaviour();
	
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

	// Customers waiting to enter the shop
	std::queue<CustomerBehaviour*> Q;
	void dequeueCustomer();
	bool buildingIsAtCapacity();
	int customerCount;
	
	std::vector<Furnishing*> furnishings;
	std::vector<DispatchingBehaviour*> tryoutBehaviours;
	std::vector<ShopKeeperBehaviour*> shopKeeperBehaviours;
	
//	std::map<Furnishing*, Unit*> staffBindings;
	
	// Info on building types, saved as static members for private use by Building & its instances.
	static std::map<std::string, struct buildingInfo> buildingTypes;	// e.g. "pieshop" => struct buildingInfo { }
	static W::Colour defaultColour;
	static W::Colour defaultHoverColour;
	
	static std::vector<std::map<Furnishing*, Unit*>::iterator> _ind_array;
};

#endif
