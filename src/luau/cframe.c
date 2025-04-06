#include "luau/cframe.h"
#include "luau/vector3.h"
#include "cframe.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(luau)

static void luau_pushcframe(lua_State *L, CFrame cf)
{
    lua_newtable(L);

    lua_pushnumber(L, cf.X);
    lua_setfield(L, -2, "X");

    lua_pushnumber(L, cf.Y);
    lua_setfield(L, -2, "Y");

    lua_pushnumber(L, cf.Z);
    lua_setfield(L, -2, "Z");

    lua_pushstring(L, "LookVector");
    luau_pushvector3(L, (Vector3){-cf.R02, -cf.R12, -cf.R22});
    lua_settable(L, -3);

    lua_pushstring(L, "lookVector");
    luau_pushvector3(L, (Vector3){-cf.R02, -cf.R12, -cf.R22});
    lua_settable(L, -3);

    lua_pushstring(L, "RightVector");
    luau_pushvector3(L, (Vector3){cf.R00, cf.R10, cf.R20});
    lua_settable(L, -3);

    lua_pushstring(L, "UpVector");
    luau_pushvector3(L, (Vector3){cf.R01, cf.R11, cf.R21});
    lua_settable(L, -3);
}

int luau_CFrame_new(lua_State *L)
{
    CFrame ret = {0};

    if (lua_gettop(L) == 1)
    {
        Vector3 pos = luau_tovector3(L, 1);
        ret.X = pos.x;
        ret.Y = pos.y;
        ret.Z = pos.z;
    }
    else if (lua_gettop(L) == 2)
    {
        FIXME("lookat constructor %p\n", L);
    }
    else if (lua_gettop(L) == 3)
    {
        ret.X = lua_tonumber(L, 1);
        ret.Y = lua_tonumber(L, 2);
        ret.Z = lua_tonumber(L, 3);
    }
    else if (lua_gettop(L) == 7)
    {
        FIXME("quaternion constructor %p\n", L);
    }
    else if (lua_gettop(L) == 12)
    {
        ret.X = lua_tonumber(L, 1);
        ret.Y = lua_tonumber(L, 2);
        ret.Z = lua_tonumber(L, 3);
        ret.R00 = lua_tonumber(L, 4);
        ret.R01 = lua_tonumber(L, 5);
        ret.R02 = lua_tonumber(L, 6);
        ret.R10 = lua_tonumber(L, 7);
        ret.R11 = lua_tonumber(L, 8);
        ret.R12 = lua_tonumber(L, 9);
        ret.R20 = lua_tonumber(L, 10);
        ret.R21 = lua_tonumber(L, 11);
        ret.R22 = lua_tonumber(L, 12);
    }

    luau_pushcframe(L, ret);
    return 1;
}


