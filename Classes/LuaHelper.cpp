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
		W::log(s.c_str());
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
		W::log(s.c_str());
		return false;
	}
	return true;
}
bool LuaHelper::pushtable(const char *key)
{										// S: ~
	lua_getglobal(LuaInstance, key);	// -1 table
	bool error = !lua_istable(LuaInstance, -1);
	if (error)							
		lua_pop(LuaInstance, 1);
	return !error;						// S: -1 table (or if error: ~)
}
bool LuaHelper::pushSubtable(const char *key)
{										// S: -1 supertable
	if (!lua_istable(LuaInstance, -1))
		return false;
	lua_pushstring(LuaInstance, key);	// S: -1 key; -2 supertable
	lua_gettable(LuaInstance, -2);		// S: -1 subtable; -2 supertable
	bool error = !lua_istable(LuaInstance, -1);
	if (error)
		lua_pop(LuaInstance, 1);
	return !error;						// S: -1 subtable; -2 supertable (or if error: -1 supertable)
}
std::string LuaHelper::stackdump()
{
	std::string s;
	lua_State *L = LuaInstance;
	int i, n = lua_gettop(L);
	
    /*strRet.append("total in stack: ");
	strRet.append*/ //fix this for string later
	
	for (i = n; i > 0; i--) {
		int t = lua_type(L, i);
        switch (t) {
			case LUA_TNUMBER:
				char c[250];
				sprintf(c, "%.0f", lua_tonumber(L, i));
				s.append(c);
				break;
			case LUA_TSTRING:
				s.append("\"");
				s.append(lua_tostring(L, i));
				s.append("\"");
                break;
            case LUA_TBOOLEAN:
				s.append(lua_toboolean(L, i) ? "true" : "false");
                break;
            default:
				s.append("[");
				s.append(lua_typename(L, t));
				s.append("]");
                break;
        }
		s.append(" ");
    }
    //printf("\n");  /* end the listing */
	return s;
}
