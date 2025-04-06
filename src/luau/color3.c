#include "luau/color3.h"

Color3 luau_tocolor3(lua_State *L, int idx)
{
    Color3 ret;

    lua_getfield(L, idx, "R");
    ret.R = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, idx, "G");
    ret.G = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, idx, "B");
    ret.B = lua_tonumber(L, -1);
    lua_pop(L, 1);

    return ret;
}

static int luau_Color3_Lerp(lua_State *L)
{
    Color3 c1 = luau_tocolor3(L, 1);
    Color3 c2 = luau_tocolor3(L, 2);
    float t = lua_tonumber(L, 3);

    Color3 ret = {
        Lerp(c1.R, c2.R, t),
        Lerp(c1.G, c2.G, t),
        Lerp(c1.B, c2.B, t)
    };

    luau_pushcolor3(L, ret);
    return 1;
}

void luau_pushcolor3(lua_State *L, Color3 c)
{
    lua_newtable(L);

    lua_pushnumber(L, c.R);
    lua_setfield(L, -2, "R");

    lua_pushnumber(L, c.G);
    lua_setfield(L, -2, "G");

    lua_pushnumber(L, c.B);
    lua_setfield(L, -2, "B");

    lua_pushcfunction(L, luau_Color3_Lerp, "Color3:lerp");
    lua_setfield(L, -2, "lerp");
}

int luau_Color3_new(lua_State *L)
{
    float r = lua_tonumber(L, 1);
    float g = lua_tonumber(L, 2);
    float b = lua_tonumber(L, 3);

    Color3 col = {r, g, b};

    luau_pushcolor3(L, col);
    return 1;
}

int luau_Color3_fromRGB(lua_State *L)
{
    float r = lua_tonumber(L, 1);
    float g = lua_tonumber(L, 2);
    float b = lua_tonumber(L, 3);

    Color3 col = {r/255, g/255, b/255};

    luau_pushcolor3(L, col);
    return 1;
}

int luau_Color3_fromHex(lua_State *L)
{
    const char *hexstr = lua_tostring(L, 1);
    uint32_t hex = strtoul(hexstr+1, NULL, 16);

    float r = (hex>>16)&0xFF;
    float g = (hex>>8)&0xFF;
    float b = (hex>>0)&0xFF;

    Color3 col = {r/255, g/255, b/255};

    luau_pushcolor3(L, col);
    return 1;
}


