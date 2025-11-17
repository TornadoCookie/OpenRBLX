#include "luau/font.h"
#include "debug.h"

DEFAULT_DEBUG_CHANNEL(luau);

int luau_Font_new(lua_State *L)
{
    lua_newtable(L);

    lua_pushvalue(L, 1);
    lua_setfield(L, -2, "Family");

    lua_pushvalue(L, 2);
    lua_setfield(L, -2, "Weight");

    lua_pushvalue(L, 3);
    lua_setfield(L, -2, "Style");

    return 1;
}

int luau_Font_fromEnum(lua_State *L)
{
    int num = lua_tointeger(L, 1);

    lua_newtable(L);

    if (num == 46) // BuilderSans
    {
        lua_pushstring(L, "rbxasset://fonts/families/BuilderSans.json");
        lua_setfield(L, -2, "Family");
    }

    return 1;
}

