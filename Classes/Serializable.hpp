/*
 * Serializable.hpp - base class providing auto-serialization
 *
 */

#ifndef __DBT__Serializable
#define __DBT__Serializable

#include <map>
#include <vector>
#include "LuaObj.h"

class SerializerBase;

class Serializable {
protected:
	typedef std::map<std::string, SerializerBase*> serialization_descriptor;
	typedef std::vector<serialization_descriptor*> sdvec;
	typedef serialization_descriptor::iterator sdit;
	virtual void getSDs(sdvec &) = 0;
		// Subclasses should override to return their SDs.
		// Note: it is up to the subclass chain to construct a vector
		// of all the serialization descriptors in said chain.
	virtual void deserializeAdditionalProperties(LuaObj &o) { }
		// Override to deserialize properties that cannot be deserialized
		// automatically, i.e. Type-Dependent Subclass Properties
	virtual void getAdditionalSerializedProperties(std::map<std::string, std::string> &m) { }
		// Override to return serialized versions of TDSPs in the form
		// name => serialized property
		// (braces are added automatically around the latter)
	bool deserialized;
	
public:
	Serializable() : deserialized(false) { }
	std::string serialize();
	void deserialize(LuaObj &luaObj);
};

#endif
