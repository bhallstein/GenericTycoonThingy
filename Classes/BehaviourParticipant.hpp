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
		for (std::vector<std::string>::iterator it = compBehs->begin(); it < compBehs->end(); it++)
			if (*it == bTypestring)
				return true;
		return false;
	}
	bool readyForBehaviour(const char *bTypestring) {
		if (!available) return false;
		return capableOfBehaviour(bTypestring);
	}
	void capture() {
		if (!available) {
			char s[200];
			sprintf(s, "Attempt to capture unavailable BehaviourParticipant %p", this);
			throw W::Exception(s);
		}
		available = false;
	}
	void release() { available = true; }
	bool isAvailable() { return available; }
	
protected:
	virtual std::vector<std::string>* getCompatibleBehaviours() = 0;
	bool available;
};

#endif
