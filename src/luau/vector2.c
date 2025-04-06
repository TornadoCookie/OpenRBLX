#include "luau/vector2.h"

Vector2 luau_tovector2(lua_State *L, int idx)
{
    Vector2 ret = { 0 };

    if (idx < 0) idx--;

    lua_pushstring(L, "X");
    lua_gettable(L, idx);
    ret.x = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L, "Y");
    lua_gettable(L, idx);
    ret.y = lua_tonumber(L, -1);
    lua_pop(L, 1);

    return ret;
}

void luau_pushvector2(lua_State *L, Vector2 v)
{
    lua_newtable(L);

    lua_pushnumber(L, v.x);
    lua_setfield(L, -2, "X");

    lua_pushnumber(L, v.y);
    lua_setfield(L, -2, "Y");
}

int luau_Vector2_new(lua_State *L)
{
    float x = 0, y = 0;

    if (lua_gettop(L) == 2)
    {
        x = lua_tonumber(L, 1);
        y = lua_tonumber(L, 2);
    }

    luau_pushvector2(L, (Vector2){x, y});

    return 1;
}

