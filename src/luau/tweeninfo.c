#include "luau/tweeninfo.h"

int luau_TweenInfo_new(lua_State *L)
{
    lua_newtable(L);
    
    lua_pushnumber(L, lua_tonumber(L, 1));
    lua_setfield(L, -2, "Time");

    lua_pushinteger(L, lua_tointeger(L, 2));
    lua_setfield(L, -2, "EasingStyle");

    lua_pushinteger(L, lua_tointeger(L, 3));
    lua_setfield(L, -2, "EasingDirection");

    lua_pushnumber(L, lua_tonumber(L, 4));
    lua_setfield(L, -2, "RepeatCount");

    lua_pushboolean(L, lua_toboolean(L, 5));
    lua_setfield(L, -2, "Reverses");

    lua_pushnumber(L, lua_tonumber(L, 6));
    lua_setfield(L, -2, "DelayTime");

    return 1;
}

