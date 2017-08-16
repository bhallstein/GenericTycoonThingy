/*
 * BehaviourParticipant.hpp
 *
 */

#ifndef BEHAVIOURPARTICIPANT_H
#define BEHAVIOURPARTICIPANT_H

#include "types.hpp"
#include "Behaviour.hpp"

class BehaviourParticipant {
public:
	BehaviourParticipant() : available(true) { }
	bool capableOfBehaviour(const char *bTypestring) {
		std::vector<std::string> *compBehs = getCompatibleBehaviours();
		for (int i=0, n = compBehs->size(); i < n; i++)
			if (compBehs->at(i) == bTypestring)
				return true;
		return false;
	}
	bool readyForBehaviour(const char *bTypestring) {
		if (!available) return false;
		return capableOfBehaviour(bTypestring);
	}
	void capture() {
		if (!available) throw MsgException("Attempt to capture unavailable BehaviourParticipant");
		available = false;
	}
	void release() { available = true; }
	bool isAvailable() { return available; }
	
protected:
	virtual std::vector<std::string>* getCompatibleBehaviours() = 0;
	bool available;
};

#endif
