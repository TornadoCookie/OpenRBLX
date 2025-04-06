#include "luau/rect.h"
#include "luau/vector2.h"

#include <string.h>

#include "lualib.h"

static int luau_Rect__index(lua_State *L)
{
    const char *key = lua_tostring(L, 2);

    lua_getfield(L, 1, "Min");
    Vector2 min = luau_tovector2(L, -1);

    lua_getfield(L, 1, "Max");
    Vector2 max = luau_tovector2(L, -1);

    if (!strcmp(key, "Width"))
    {
        lua_pushnumber(L, max.x - min.x);
        return 1;
    }
    else if (!strcmp(key, "Height"))
    {
        lua_pushnumber(L, max.y - min.y);
        return 1;
    }
}

int luau_Rect_new(lua_State *L)
{
    Vector2 min;
    Vector2 max;

    if (lua_gettop(L) == 0)
    {
        min = (Vector2){0, 0};
        max = (Vector2){0, 0};
    }
    else if (lua_gettop(L) == 2)
    {
        min = luau_tovector2(L, 1);
        max = luau_tovector2(L, 2);
    }
    else if (lua_gettop(L) == 4)
    {
        min.x = lua_tonumber(L, 1);
        min.y = lua_tonumber(L, 2);
        max.x = lua_tonumber(L, 3);
        max.y = lua_tonumber(L, 4);
    }
    else
    {
        luaL_error(L, "Invalid argument count to Rect.new\n");
    }

    lua_newtable(L);

    lua_newtable(L);

    lua_pushcfunction(L, luau_Rect__index, "Rect:__index");
    lua_setfield(L, -2, "__index");

    lua_setmetatable(L, -2);

    luau_pushvector2(L, min);
    lua_setfield(L, -2, "Min");

    luau_pushvector2(L, max);
    lua_setfield(L, -2, "Max");

    return 1;
}


