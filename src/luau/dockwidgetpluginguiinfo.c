#include "luau/dockwidgetpluginguiinfo.h"
#include <stdbool.h>

int luau_DockWidgetPluginGuiInfo_new(lua_State *L)
{
    int InitialDockState = lua_tointeger(L, 1);
    bool InitialEnabled = lua_toboolean(L, 2);
    bool InitialEnabledShouldOverrideRestore = lua_toboolean(L, 3);
    float FloatingXSize = lua_tonumber(L, 4);
    float FloatingYSize = lua_tonumber(L, 5);
    float MinWidth = lua_tonumber(L, 6);
    float MinHeight = lua_tonumber(L, 7);

    lua_newtable(L);

#define set(n, T) lua_pushstring(L, #n); lua_push##T(L, n); lua_settable(L, -3);

    set(InitialDockState, integer);
    set(InitialEnabled, boolean);
    set(InitialEnabledShouldOverrideRestore, boolean);
    set(FloatingXSize, number);
    set(FloatingYSize, number);
    set(MinWidth, number);
    set(MinHeight, number);

#undef set

    return 1;
}

