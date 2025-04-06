#include "luau/udim.h"

UDim luau_toudim(lua_State *L, int idx)
{
    UDim ret;

    lua_getfield(L, idx, "Scale");
    ret.Scale = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, idx, "Offset");
    ret.Offset = lua_tonumber(L, -1);
    lua_pop(L, 1);

    return ret;
}

void luau_pushudim(lua_State *L, UDim udim)
{
    lua_newtable(L);
    
    lua_pushnumber(L, udim.Scale);
    lua_setfield(L, -2, "Scale");

    lua_pushnumber(L, udim.Offset);
    lua_setfield(L, -2, "Offset");
}

int luau_UDim_new(lua_State *L)
{
    float scale = lua_tonumber(L, 1);
    float offset = lua_tonumber(L, 2);

    luau_pushudim(L, (UDim){scale, offset});   

    return 1;
}

