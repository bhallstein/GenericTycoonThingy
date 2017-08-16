/* 
 * LuaHelper.hpp - C++ functions to make Lua interaction more elegant
 *
 */

#ifndef LUAHELPER_H
#define LUAHELPER_H

#include <string>
#include <iostream>

#include <Lua.hpp>

class W;

class LuaHelper {
public:
	LuaHelper(W *);
	~LuaHelper();

	bool loadFile(std::string &fileName);

//lua helper function templates
	int pushtable(const char *key);
	int pushSubtable(const char *key);

	template <typename T>
		T to(int index);

	template <typename T>
		T getfield(const char *table, const char *key);
	template <typename T>
		T getfield(const char *key);

	template <typename T>
		T getSubfield(const char *table, const char *key);

	template <typename T>
		T getvalue(const char *key);

	lua_State *LuaInstance; //instance of the Lua Interpreter

	//stackdump
	void stackdump(lua_State *l);
	
	W *theW;
};

//template implementations

//get a basic field from a lua table
template <typename T>
inline T LuaHelper::getfield(const char *table, const char *key) //can return int,double,string,char[],bool
{
	T result;
	lua_getglobal(LuaInstance, table); //push the table to the stack
	lua_pushstring(LuaInstance, key); //push the key to the stack
	lua_gettable(LuaInstance, -2);  //get table[key] - pop the key and push the value to the stack
	result = to<T>(-1); //convert the value to the requested type if possible
	lua_pop(LuaInstance, 2);  //pop the value and the table
	return result;
}
//get a basic field from a lua table
template <typename T>
inline T LuaHelper::getfield(const char *key) //can return int,double,string,char[],bool
{
	T result;
	lua_pushstring(LuaInstance, key); //push the key to the stack
	lua_gettable(LuaInstance, -2);  //get table[key] - pop the key and push the value to the stack
	result = to<T>(-1); //convert the value to the requested type if possible
	lua_pop(LuaInstance, 1);  //pop the value and the table
	return result;
}
//get a basic field from a lua table
template <typename T>
inline T LuaHelper::getSubfield(const char *table, const char *key) //can return int,double,string,char[],bool
{
	T result;
	pushSubtable(table);
	lua_pushstring(LuaInstance, key); //push the key to the stack
	lua_gettable(LuaInstance, -2);  //get table[key] - pop the key and push the value to the stack
	result = to<T>(-1); //convert the value to the requested type if possible
	lua_pop(LuaInstance, 2);  //pop the value and the table
	return result;
}
//get a basic value from a lua global variable
template <typename T>
inline T LuaHelper::getvalue(const char *key) //int,double,string,char[],bool
{
	T result;
	lua_getglobal(LuaInstance,key); //push to the stack
	result = to<T>(-1); //convert the value to the requested type if possible
	lua_pop(LuaInstance, 1); //pop from the stack
	return result;
}

//Lua to<T> type specific definitions
template<>
	inline bool LuaHelper::to<bool>(int index) { return lua_toboolean(LuaInstance, index); }
template<>
	inline int LuaHelper::to<int>(int index) { return lua_tointeger(LuaInstance, index); }
template<>
	inline double LuaHelper::to<double>(int index) { return lua_tonumber(LuaInstance, index); }
template<>
	inline const char* LuaHelper::to<const char*>(int index) { return lua_tostring(LuaInstance, index); }
template<>
	inline std::string LuaHelper::to<std::string>(int index) { return lua_tostring(LuaInstance, index); }

#endif
