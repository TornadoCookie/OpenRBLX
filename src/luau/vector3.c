#include "luau/vector3.h"

Vector3 luau_tovector3(lua_State *L, int idx)
{
    Vector3 ret = { 0 };

    if (idx < 0) idx--;

    lua_pushstring(L, "X");
    lua_gettable(L, idx);
    ret.x = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L, "Y");
    lua_gettable(L, idx);
    ret.y = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L, "Z");
    lua_gettable(L, idx);
    ret.z = lua_tonumber(L, -1);
    
    lua_pop(L, 1);

    return ret;
}

static int luau_vector3__mul(lua_State *L)
{
    lua_Number factor = lua_tonumber(L, 2);
    Vector3 vec = luau_tovector3(L, 1);

    Vector3 ret = (Vector3){
        vec.x * factor,
        vec.y * factor,
        vec.z * factor
    };

    luau_pushvector3(L, ret);

    return 1;
}

static int luau_vector3__add(lua_State *L)
{
    Vector3 v2 = luau_tovector3(L, 2);
    Vector3 v1 = luau_tovector3(L, 1);

    Vector3 ret = Vector3Add(v1, v2);

    luau_pushvector3(L, ret);

    return 1;
}

void luau_pushvector3(lua_State *L, Vector3 v)
{
    lua_newtable(L);

    lua_newtable(L);

    lua_pushcfunction(L, luau_vector3__mul, "Vector3:__mul");
    lua_setfield(L, -2, "__mul");

    lua_pushcfunction(L, luau_vector3__add, "Vector3:__add");
    lua_setfield(L, -2, "__add");

    lua_setmetatable(L, -2);

    lua_pushnumber(L, v.x);
    lua_setfield(L, -2, "X");

    lua_pushnumber(L, v.y);
    lua_setfield(L, -2, "Y");

    lua_pushnumber(L, v.z);
    lua_setfield(L, -2, "Z");
}

int luau_Vector3_new(lua_State *L)
{
    Vector3 ret = {0};

    if (lua_gettop(L) == 3)
    {
        ret.x = lua_tonumber(L, 1);
        ret.y = lua_tonumber(L, 2);
        ret.z = lua_tonumber(L, 3);
    }

    luau_pushvector3(L, ret);

    return 1;
}

