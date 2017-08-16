#include "UIDManager.hpp"
#include "TLO.hpp"

TLO* UID::get() {
	return UIDManager::getTLO(*this);
}


unsigned int UIDManager::currentID = 1;
std::map<UID,UID> UIDManager::translationMap;
std::map<UID,TLO*> UIDManager::lookupMap;

void UIDManager::newBatch() {
	translationMap.clear();
}

void UIDManager::translateLoadedUID(UID &orig_uid) {
	UID new_uid;
	idMap::iterator it = translationMap.find(orig_uid);
	if (it == translationMap.end())
		translationMap[orig_uid] = new_uid = getNewUID();
	else
		new_uid = translationMap[orig_uid];
	orig_uid = new_uid;
}

void UIDManager::registerTLO(TLO *t) {
	lookupMap[t->uid] = t;
}

void UIDManager::unregisterTLO(TLO *t) {
	lookupMap.erase(t->uid);
}

TLO* UIDManager::getTLO(const UID &uid) {
	tloMap::iterator it = lookupMap.find(uid);
	return (it == lookupMap.end() ? NULL : it->second);
}

void UIDManager::reset() {
	lookupMap.clear();
	currentID = 0;
}
