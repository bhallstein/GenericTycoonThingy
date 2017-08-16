/*
 * UIDManager.hpp - static class allowing TLOs to refer to each other by UID
 *
 * 
 *
 */

#ifndef __DBT__UIDManager__
#define __DBT__UIDManager__

#include "types.hpp"
#include <map>

struct UID {
	UID(unsigned int _id) {
		id = _id;
	}
	UID() : id(0) { }
	unsigned int id;
	bool operator== (const UID &u) const { return id == u.id; }
	bool operator<  (const UID &u) const { return id < u.id; }
	bool operator>  (const UID &u) const { return id > u.id; }
	bool operator<= (const UID &u) const { return id <= u.id; }
	bool operator>= (const UID &u) const { return id >= u.id; }
};


class TLO;

class UIDManager {
public:
	// When loading a batch of TLOs, their UIDs may conflict with the UIDs of
	// TLOs already existing in the level.
	//  - Therefore, we translate all loaded UIDs in the batch into a set of new
	//    ones. This requires a translation table of old -> new ids for the current batch.
	//  - So, before loading a batch, the table must be cleared by calling newBatch().
	static void newBatch();
	
	// If the passed uid has no entry in the translation table, we add one.
	// Then, we change the passed uid to its translated value.
	static void translateLoadedUID(UID &);

	// Get a fresh, unused UID
	static UID getNewUID() {
		return UID(currentID++);
	}
	
	// Register a TLO by its UID in the lookup table
	static void registerTLO(TLO *);
	
	// Remove an entry from the lookup table (i.e. when a TLO is destroyed)
	static void unregisterTLO(TLO *);
	
	// Return a TLO pointer if the UID is found, otherwise NULL
	static TLO* getTLO(const UID &);
	
	// Reset the UIDManager's id counter and lookup map
	// (when starting a new level, for instance)
	static void reset();
	
private:
	UIDManager() { }
	
	static unsigned int currentID;

	typedef std::map<UID,UID> idMap;
	static idMap translationMap;
	
	typedef std::map<UID,TLO*> tloMap;
	static tloMap lookupMap;
};

#endif