/*
 * Generic Tycoon Thingy
 *
 * ====================
 *  Serializable.cpp
 * ====================
 *
 * Copyright (C) 2012 - Ben Hallstein, Jon Couldridge & Philip Berry
 * All rights reserved
 *
 */

#include "Serializable.hpp"
#include "Serializer.hpp"

std::string Serializable::serialize() {
	using std::string;
	using std::vector;
	using std::map;
	
	sdvec sds;
	getSDs(sds);
	
	string s;
	vector<string> v;
	
	for (sdvec::iterator it = sds.begin(); it < sds.end(); ++it) {
		serialization_descriptor &sd = **it;
		for (sdit it2 = sd.begin(); it2 != sd.end(); ++it2)
			v.push_back(
				it2->first + string(" = ") + it2->second->serialize(this)
			);
	}

	map<string,string> m;
	getAdditionalSerializedProperties(m);
	for (map<string, string>::iterator it = m.begin(); it != m.end(); ++it)
		v.push_back(
			it->first + string(" = {\n") + it->second + string("}")
		);
	
	for (vector<string>::iterator it = v.begin(), itend = v.end(); it < itend; it++)
		s += *it + (it == itend-1 ? "\n" : ",\n");
	return s;
}

void Serializable::deserialize(LuaObj &luaObj) {
	sdvec sds;
	getSDs(sds);
	
	for (sdvec::iterator it = sds.begin(); it < sds.end(); ++it) {
		serialization_descriptor &sd = **it;
		for (sdit it2 = sd.begin(); it2 != sd.end(); ++it2) {
			LuaObj &lo = luaObj[it2->first];
			if (lo.isNil())
				; // error: property doesn't exist in supplied LuaObj
			it2->second->deserialize(lo, this);
		}
	}
	
	deserializeAdditionalProperties(luaObj);
	
	deserialized = true;
}
