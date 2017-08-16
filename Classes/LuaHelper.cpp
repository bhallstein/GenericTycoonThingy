#include "LuaHelper.hpp"

LuaHelper::LuaHelper() {
	LuaInstance = luaL_newstate();
	luaL_openlibs(LuaInstance);
}
LuaHelper::~LuaHelper() {
	lua_close(LuaInstance);
}

int LuaHelper::loadFile(std::string fileName)
{
	if (luaL_loadfile(LuaInstance, fileName.c_str()) || lua_pcall(LuaInstance, 0, 0, 0))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
//push a table onto the stack, or error if nil;
int LuaHelper::pushtable(const char *key)
{
	lua_getglobal(LuaInstance, key); //push the table to the stack
	if(lua_isnil(LuaInstance,-1))
	{
		lua_pop(LuaInstance,1);
		return 1;
	}
	else
	{
		return 0;
	}
}
int LuaHelper::pushSubtable(const char *key)
{
	lua_pushstring(LuaInstance, key); //push the key to the stack
	lua_gettable(LuaInstance, -2);  //get table[key] - pop the key and push the value to the stack
	if(lua_isnil(LuaInstance,-1))
	{
		lua_pop(LuaInstance,1);
		return 1;
	}
	else
	{
		return 0;
	}
}