#include "Building.h"

Building::Building() {
	// initialize building
	// see LevelMap.cpp for part of this process
}
Building::~Building()
{
	//deconstructor
}



#ifdef yeah_this_doesnt_need_compiling_either_sorry

  What is a building?
  
  A building is an area displayed at a paticular location on the map.
  When it is created, the user has the chance to specify where it can be built.
  	- some parts of the map may be buildable, others not
  
  A building also defines behaviours of some other objects.
  A staff unit may seek out or be assigned to a building
  
  A building generates its own internal mapping region, also – i.e. they define a free movement zone,
  and form a node in the pathfinding system’s nodelist.
  
  
  Buildings are like other things in the game, like units, obstacles, implements, furniture in
  some key respects – each object must have a sprite, and the code for managing the display of that sprite
  (or returning it for display by some more powerful, parent drawing class.)

#endif