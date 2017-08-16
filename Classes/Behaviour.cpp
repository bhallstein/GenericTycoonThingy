#include "Behaviour.hpp"
#include "Unit.hpp"
#include "Building.hpp"
#include "Furnishing.hpp"
#include "LevelState.hpp"
#include "MrPaths.hpp"
#include "MrKlangy.hpp"
#include "types.hpp"


Serializable::serialization_descriptor Behaviour::sd;
Serializable::serialization_descriptor CustomerBehaviour::sd;


/* CustomerBehaviour impl */

void CustomerBehaviour::update() {
	/* Wander between random locations */
	if (stage == 0) {
		++stage;
		failureStage = 100;
		u->wanderToRandomMapDestination();
	}
	else if (stage == 1) {
		// wait for unit to arrive at destination
	}
	else if (stage == 2) {
		// pause for ~2s
		if (++timeWaited >= 120) {
			timeWaited = 0;
			stage = 0;
		}
	}
	
	/* Failure: wander to a random destination */
	else if (stage == 100) {
		stage = 2;	// Wait 2s then try again
	}
}
