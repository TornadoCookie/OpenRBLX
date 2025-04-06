#include "luau/udim2.h"
#include "luau/udim.h"
#include "udim2.h"

UDim2 luau_toudim2(lua_State *L, int idx)
{
    UDim2 ret;

    lua_getfield(L, idx, "X");
    ret.X = luau_toudim(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, idx, "Y");
    ret.Y = luau_toudim(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, idx, "Width");
    ret.Width = luau_toudim(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, idx, "Height");
    ret.Height = luau_toudim(L, -1);
    lua_pop(L, 1);

    return ret;
}

static int luau_UDim2__sub(lua_State *L)
{
    UDim2 u1 = luau_toudim2(L, 1);
    UDim2 u2 = luau_toudim2(L, 2);
    UDim2 ret;

    ret.X.Scale = u1.X.Scale - u2.X.Scale;
    ret.X.Offset = u1.X.Offset - u2.X.Offset;
    ret.Y.Scale = u1.Y.Scale - u2.Y.Scale;
    ret.Y.Offset = u1.Y.Offset - u2.Y.Offset;
    ret.Width.Scale = u1.Width.Scale - u2.Width.Scale;
    ret.Width.Offset = u1.Width.Offset - u2.Width.Offset;
    ret.Height.Scale = u1.Height.Scale - u2.Height.Scale;
    ret.Height.Offset = u1.Height.Offset - u2.Height.Offset;

    luau_pushudim2(L, ret);
    return 1;
}

void luau_pushudim2(lua_State *L, UDim2 udim2)
{
    lua_newtable(L);

    lua_newtable(L);

    lua_pushcfunction(L, luau_UDim2__sub, "UDim2.__sub");
    lua_setfield(L, -2, "__sub");

    lua_setmetatable(L, -2);

    luau_pushudim(L, udim2.X);
    lua_setfield(L, -2, "X");

    luau_pushudim(L, udim2.Y);
    lua_setfield(L, -2, "Y");

    luau_pushudim(L, udim2.Width);
    lua_setfield(L, -2, "Width");

    luau_pushudim(L, udim2.Height);
    lua_setfield(L, -2, "Height");
}

int luau_UDim2_fromScale(lua_State *L)
{
    UDim2 udim2 = {0};

    udim2.X.Scale = lua_tonumber(L, 1);
    udim2.Y.Scale = lua_tonumber(L, 2);

    luau_pushudim2(L, udim2);
    return 1;
}

int luau_UDim2_fromOffset(lua_State *L)
{
    UDim2 udim2 = {0};

    udim2.X.Offset = lua_tonumber(L, 1);
    udim2.Y.Offset = lua_tonumber(L, 2);

    luau_pushudim2(L, udim2);
    return 1;
}

int luau_UDim2_new(lua_State *L)
{
    UDim2 udim2 = {0};

    if (lua_gettop(L) == 4)
    {
        udim2.X.Scale = lua_tonumber(L, 1);
        udim2.X.Offset = lua_tonumber(L, 2);
        udim2.Y.Scale = lua_tonumber(L, 3);
        udim2.Y.Offset = lua_tonumber(L, 4);
    }

    luau_pushudim2(L, udim2);
    return 1;
}


