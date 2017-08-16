#include "LuaHelper.hpp"
#include "../W.hpp"

LuaHelper::LuaHelper(W *_theW) : theW(_theW) {
	LuaInstance = luaL_newstate();
	luaL_openlibs(LuaInstance);
}
LuaHelper::~LuaHelper() {
	lua_close(LuaInstance);
}

bool LuaHelper::loadFile(const char *filename)
{
	bool loadingSuccess = !luaL_loadfile(LuaInstance, filename);
	if (!loadingSuccess) {
		std::string s = "LuaHelper: could not load file '", t; s.append(filename); s.append("'; error was: ");
		to<std::string>(-1, t); s.append(t);
		theW->log(s.c_str());
		return false;
	}
	// Add our Resources folder to the Lua path
	lua_getglobal(LuaInstance, "package");
	lua_getfield(LuaInstance, -1, "path");
	std::string path = lua_tostring(LuaInstance, -1);	// Grab path string from top of stack
	path.append(";"); path.append(theW->resourcesPath); path.append("?.lua");
	lua_pop(LuaInstance, 1);					// Pop previous path from stack
	lua_pushstring(LuaInstance, path.c_str());	// Push new path onto stack
	lua_setfield(LuaInstance, -2, "path");		// Set the "path" of table at -2 to value at top of stack (value is then popped)
	lua_pop(LuaInstance, 1);					// Pop "package" table from top of stack
	
	bool callingSuccess = !lua_pcall(LuaInstance, 0, 0, 0);
	if (!callingSuccess) {
		std::string s = "LuaHelper: could not execute file '", t; s.append(filename); s.append("'; error was: ");
		to<std::string>(-1, t); s.append(t);
		theW->log(s.c_str());
		return false;
	}
	return true;
}
//push a table onto the stack, or error if nil;
bool LuaHelper::pushtable(const char *key)
{
	lua_getglobal(LuaInstance, key); //push the table to the stack
	bool error = lua_isnil(LuaInstance, -1);
	if (error)
		lua_pop(LuaInstance, 1);
	return !error;
}
bool LuaHelper::pushSubtable(const char *key)
{
	lua_pushstring(LuaInstance, key); //push the key to the stack
	lua_gettable(LuaInstance, -2);  //get table[key] - pop the key and push the value to the stack
	bool error = lua_isnil(LuaInstance, -1);
	if (error)
		lua_pop(LuaInstance, 1);
	return !error;
}
std::string LuaHelper::stackdump(lua_State* l)
{
	std::string strRet;
    int i, top = lua_gettop(l);
 
    /*strRet.append("total in stack: ");
	strRet.append*/ //fix this for string later
 
    for (i = 1; i <= top; i++)
    {  /* repeat for each level */
        int t = lua_type(l, i);
        switch (t) {
			case LUA_TNUMBER:
            case LUA_TSTRING:  /* strings and numbers */
                strRet.append("string: ");
				strRet.append(lua_tostring(l, i));
                break;
            case LUA_TBOOLEAN:  /* booleans */
				strRet.append("boolean: ");
				strRet.append(lua_toboolean(l, i) ? "true" : "false");
                break;
            /*case LUA_TNUMBER:  /* numbers 
                printf("number: %g\n", lua_tonumber(l, i));
                break;*/
            default:  /* other values */
				strRet.append(lua_typename(l, t));
                break;
        }
		strRet.append("\n");
    }
    //printf("\n");  /* end the listing */
	return strRet;
}
