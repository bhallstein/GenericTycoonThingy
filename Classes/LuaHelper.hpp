/* 
 * LuaHelper.hpp - C++ functions to make Lua interaction more elegant
 *
 */

#ifndef LUAHELPER_H
#define LUAHELPER_H

#include <string>
#include <iostream>

#include <Lua.hpp>
#include "types.hpp"

class W;

class LuaHelper {
public:
	LuaHelper(W *);
	~LuaHelper();

	bool loadFile(const char *);

//lua helper function templates
	bool pushtable(const char *key);
	bool pushSubtable(const char *key);

	template <typename T> T getfield(const char *table, const char *key);
	template <typename T> T getfield(const char *key);
	template <typename T> T getfield(int numerical_key);
	template <typename T> T getSubfield(const char *table, const char *key);
	template <typename T> T getvalue(const char *key);
	
	template <typename T> bool to(int index, T&);
	
	lua_State *LuaInstance;
	std::string stackdump();
	
	W *theW;
};

//template implementations

//get a basic field from a lua table
template <typename T>
inline T LuaHelper::getfield(const char *table, const char *key) //can return int,double,string,char[],bool
{
	T val;
	lua_getglobal(LuaInstance, table);	// S: -1 table
	lua_pushstring(LuaInstance, key);	// S: -1 key; -2 table
	lua_gettable(LuaInstance, -2);		// S: -1 value; -2 table
	bool success = to<T>(-1, val);
	lua_pop(LuaInstance, 2);			// S: ~
	if (!success) throw MsgException("Unexpected Lua type encountered.");
	return val;
}
//get a basic field from a lua table, assuming table is at index -1
template <typename T>
inline T LuaHelper::getfield(const char *key) //can return int,double,string,char[],bool
{										// S: -1 table
	T val;
	lua_pushstring(LuaInstance, key);	// S: -1 key; -2 table
	lua_gettable(LuaInstance, -2);		// S: -1 value; -2 table
	bool success = to<T>(-1, val);
	lua_pop(LuaInstance, 1);			// S: -1 table
	if (!success) throw MsgException("Unexpected Lua type encountered.");
	return val;
}
template <typename T>
inline T LuaHelper::getfield(int numerical_key)
{										// S: -1 table
	T val;
	lua_pushnumber(LuaInstance, numerical_key);
	lua_gettable(LuaInstance, -2);		// S: -1 value; -2 table
	bool success = to<T>(-1, val);
	lua_pop(LuaInstance, 1);			// S: -1 table;
	if (!success) throw MsgException("Unexpected Lua type encountered.");
	return val;
}
//get a basic field from a lua table
template <typename T>
inline T LuaHelper::getSubfield(const char *table, const char *key) //can return int,double,string,char[],bool
{
	T val;
	bool error = !pushSubtable(table);	// S: -1 subtable
	if (error)
		throw MsgException("Nonexistent Lua subtable requested.");
	lua_pushstring(LuaInstance, key);	// S: -1 key; -2 subtable
	lua_gettable(LuaInstance, -2);		// S: -1 value; -2 subtable
	error = lua_isnil(LuaInstance, -1);
	if (error) {
		lua_pop(LuaInstance, 2);		// S: ~
		throw MsgException("Invalid key requested in Lua subtable.");
	}
	error = !to<T>(-1, val);
	lua_pop(LuaInstance, 2);			// S: ~
	if (error)
		throw MsgException("Unexpected Lua type encountered.");
	return val;
}
//get a basic value from a lua global variable
template <typename T>
inline T LuaHelper::getvalue(const char *key) //int,double,string,char[],bool
{
	T val;
	lua_getglobal(LuaInstance, key);	// S: -1 key
	bool success = to<T>(-1, val);
	lua_pop(LuaInstance, 1);			// S: ~
	if (!success) throw MsgException("Unexpected Lua type encountered.");
	return val;
}

//Lua to<T> type specific definitions
template<> inline bool LuaHelper::to<bool>(int index, bool &p) {
	if (!lua_isboolean(LuaInstance, index)) return false;
	p = lua_toboolean(LuaInstance, index);
	return true;
}
template<> inline bool LuaHelper::to<int>(int index, int &p) {
	if (!lua_isnumber(LuaInstance, index)) return false;
	p = lua_tointeger(LuaInstance, index);
	return true;
}
template<> inline bool LuaHelper::to<double>(int index, double &p) {
	if (!lua_isnumber(LuaInstance, index)) return false;
	p = lua_tonumber(LuaInstance, index);
	return true;
}
template<> inline bool LuaHelper::to<const char *>(int index, const char *&p) {
	if (!lua_isstring(LuaInstance, index)) return false;
	p = lua_tostring(LuaInstance, index);
	return true;
}
template<> inline bool LuaHelper::to<std::string>(int index, std::string &p) {
	if (!lua_isstring(LuaInstance, index)) return false;
	p = lua_tostring(LuaInstance, index);
	return true;
}

#endif
