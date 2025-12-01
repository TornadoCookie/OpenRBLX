#include "luau/brickcolor.h"
#include "luau/color3.h"
#include "debug.h"

#include <string.h>

DEFAULT_DEBUG_CHANNEL(luau);

int luau_BrickColor_new(lua_State *L)
{
    int nargs = lua_gettop(L);

    if (nargs != 1)
    {
        FIXME("%d args\n", nargs);
        return 0;
    }

    switch (lua_type(L, 1))
    {
        //case LUA_TSTRING:
        //{
        //} break;
        case LUA_TNUMBER:
        {
            luau_pushbrickcolor(L, GetBrickColorFromId(lua_tointeger(L, 1)));
            return 1;
        } break;
        //case LUA_TUSERDATA:
        //{
        //} break;
        default:
        {
            FIXME("unknown type for 1 arg: %d\n", lua_type(L, 1));
        } break;
    }

    return 0;
}

BrickColor luau_tobrickcolor(lua_State *L, int idx)
{
    BrickColor *ud = lua_touserdata(L, idx);

    return *ud;
}

static int luau_BrickColor__index(lua_State *L)
{
    BrickColor *bc = lua_touserdata(L, 1);
    const char *key = lua_tostring(L, 2);

    if (!strcmp(key, "Name"))
    {
        lua_pushstring(L, bc->name);
    }
    else if (!strcmp(key, "Number"))
    {
        lua_pushinteger(L, bc->paletteIndex);
    }
    else if (!strcmp(key, "Color"))
    {
        luau_pushcolor3(L, bc->color);
    }
    else
    {
        FIXME("no key %s\n", key);
        lua_pushnil(L);
    }

    return 1;
}

void luau_pushbrickcolor(lua_State *L, BrickColor bc)
{
    BrickColor *bud = lua_newuserdata(L, sizeof(bc));
    *bud = bc;

    lua_newtable(L);
    
    lua_pushcfunction(L, luau_BrickColor__index, "BrickColor.__index");
    lua_setfield(L, -2, "__index");

    lua_setmetatable(L, -2);
}

