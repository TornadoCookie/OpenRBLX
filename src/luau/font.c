#include "luau/font.h"

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

