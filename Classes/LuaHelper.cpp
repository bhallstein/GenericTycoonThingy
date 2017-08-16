#include "LuaHelper.hpp"

LuaHelper::LuaHelper() {
	LuaInstance = luaL_newstate();
	luaL_openlibs(LuaInstance);
}
LuaHelper::~LuaHelper() {
	lua_close(LuaInstance);
}

bool LuaHelper::loadFile(std::string &fileName)
{
	return !(luaL_loadfile(LuaInstance, fileName.c_str()) || lua_pcall(LuaInstance, 0, 0, 0));
}
//push a table onto the stack, or error if nil;
int LuaHelper::pushtable(const char *key)
{
	lua_getglobal(LuaInstance, key); //push the table to the stack
	int success = lua_isnil(LuaInstance,-1);
	if (success)
		lua_pop(LuaInstance,1);
	return success;
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
std::string LuaHelper::stackdump(lua_State* l)
{
	std::string strRet;
    int i;
    int top = lua_gettop(l);
 
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
				strRet.append("\n");
                break;
            case LUA_TBOOLEAN:  /* booleans */
				strRet.append("boolean: ");
				strRet.append(lua_toboolean(l, i) ? "true" : "false");
				strRet.append("\n");
                break;
            /*case LUA_TNUMBER:  /* numbers 
                printf("number: %g\n", lua_tonumber(l, i));
                break;*/
            default:  /* other values */
				strRet.append(lua_typename(l, t));
				strRet.append("\n");
                break;
        }
        //printf("  ");  /* put a separator */
    }
    //printf("\n");  /* end the listing */
	return strRet;
}