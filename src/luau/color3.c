#include "luau/color3.h"

#include <string.h>
#include <stdlib.h>

#include "lualib.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(luau);

#include "luau/utils.h"

Color3 luau_tocolor3(lua_State *L, int idx)
{
    Color3 *ud = lua_touserdata(L, idx);

    return *ud;
}

static int luau_Color3_Lerp(lua_State *L)
{
    luau_expectargs(L, 3);
    luau_expectargtype(L, 1, userdata);
    luau_expectargtype(L, 2, userdata);
    luau_expectargtype(L, 3, number);

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

static int luau_Color3__index(lua_State *L)
{
    Color3 *c = lua_touserdata(L, 1);
    const char *key = lua_tostring(L, 2);

    if (!strcmp(key, "R"))
    {
        lua_pushnumber(L, c->R);
    }
    else if (!strcmp(key, "G"))
    {
        lua_pushnumber(L, c->G);
    }
    else if (!strcmp(key, "B"))
    {
        lua_pushnumber(L, c->B);
    }
    else if (!strcmp(key, "lerp"))
    {
        lua_pushcfunction(L, luau_Color3_Lerp, "Color3:lerp");
    }
    else
    {
        FIXME("no key %s\n", key);
        lua_pushnil(L);
    }

    return 1;
}

void luau_pushcolor3(lua_State *L, Color3 c)
{
    Color3 *cud = lua_newuserdata(L, sizeof(c));
    *cud = c;

    lua_newtable(L);

    lua_pushcfunction(L, luau_Color3__index, "Color3.__index");
    lua_setfield(L, -2, "__index");

    lua_setmetatable(L, -2);
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


