#include "luau/state.h"
#include "luau/globals.h"
#include "luau/enum.h"
#include "luau/vector3.h"
#include "luau/vector2.h"
#include "luau/cframe.h"
#include "luau/color3.h"
#include "luau/rect.h"
#include "luau/udim.h"
#include "luau/udim2.h"
#include "luau/dockwidgetpluginguiinfo.h"
#include "luau/tweeninfo.h"
#include "luau/instance.h"

#include "datamodel.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(state);

static int luau_globals__index(lua_State *L)
{
    char *name = lua_tostring(L, 2);

    FIXME("global name %s\n", name);
}

void init_lua_state(lua_State *L, Script *script, bool client, bool plugin, Plugin *pluginObj)
{
    //luaL_openlibs(L);
    
    // Lua global functions
    lua_pushcfunction(L, luau_require_cached, "require");
    lua_setglobal(L, "require");

    lua_getglobal(L, "pcall");
    lua_setglobal(L, "__pcall");

    lua_getglobal(L, "xpcall");
    lua_setglobal(L, "__xpcall");

    lua_getglobal(L, "typeof");
    lua_setglobal(L, "__typeof");

    lua_pushcfunction(L, luau_pcall, "pcall");
    lua_setglobal(L, "pcall");

    lua_pushcfunction(L, luau_xpcall, "xpcall");
    lua_setglobal(L, "xpcall");

    lua_pushcfunction(L, luau_typeof, "typeof");
    lua_setglobal(L, "typeof");

    // Roblox global functions
    lua_pushcfunction(L, luau_wait, "wait");
    lua_setglobal(L, "wait");

    lua_pushcfunction(L, luau_tick, "tick");
    lua_setglobal(L, "tick");

    lua_pushcfunction(L, luau_warn, "warn");
    lua_setglobal(L, "warn");

    lua_pushcfunction(L, luau_settings, "settings");
    lua_setglobal(L, "settings");

    // Global instances
    luau_pushinstance(L, GetDataModel()->Workspace);
    lua_setglobal(L, "Workspace");

    luau_pushinstance(L, script);
    lua_setglobal(L, "script");

    luau_pushinstance(L, GetDataModel());
    lua_setglobal(L, "game");

    if (plugin)
    {
        luau_pushinstance(L, pluginObj);
        lua_setglobal(L, "plugin");
    }
    
    // task library
    lua_newtable(L);

    lua_pushcfunction(L, luau_task_delay, "task.delay");
    lua_setfield(L, -2, "delay");

    lua_pushcfunction(L, luau_task_defer, "task.defer");
    lua_setfield(L, -2, "defer");
    
    lua_setglobal(L, "task");

    // Global data types
    lua_newtable(L);

    lua_pushcfunction(L, luau_Enums_GetEnums, "Enums:GetEnums");
    lua_setfield(L, -2, "GetEnums");

    lua_newtable(L);
    lua_pushcfunction(L, luau_Enums__index, "Enums:__index");
    lua_setfield(L, -2, "__index");
    
    lua_setmetatable(L, -2);

    lua_setglobal(L, "Enum");

    // Data type constructors
    // Vector3
    lua_newtable(L);

    lua_pushcfunction(L, luau_Vector3_new, "Vector3.new");
    lua_setfield(L, -2, "new");

    lua_setglobal(L, "Vector3");

    // Vector2
    lua_newtable(L);

    lua_pushcfunction(L, luau_Vector2_new, "Vector2.new");
    lua_setfield(L, -2, "new");

    lua_setglobal(L, "Vector2");

    // CFrame
    lua_newtable(L);

    lua_pushcfunction(L, luau_CFrame_new, "CFrame.new");
    lua_setfield(L, -2, "new");

    lua_setglobal(L, "CFrame");

    // Color3
    lua_newtable(L);

    lua_pushcfunction(L, luau_Color3_new, "Color3.new");
    lua_setfield(L, -2, "new");

    lua_pushcfunction(L, luau_Color3_fromRGB, "Color3.fromRGB");
    lua_setfield(L, -2, "fromRGB");

    lua_pushcfunction(L, luau_Color3_fromHex, "Color3.fromHex");
    lua_setfield(L, -2, "fromHex");

    lua_setglobal(L, "Color3");

    // Instance
    lua_newtable(L);

    lua_pushcfunction(L, luau_Instance_new, "Instance.new");
    lua_setfield(L, -2, "new");

    lua_setglobal(L, "Instance");

    // Rect
    lua_newtable(L);

    lua_pushcfunction(L, luau_Rect_new, "Rect.new");
    lua_setfield(L, -2, "new");

    lua_setglobal(L, "Rect");

    // UDim
    lua_newtable(L);

    lua_pushcfunction(L, luau_UDim_new, "UDim.new");
    lua_setfield(L, -2, "new");

    lua_setglobal(L, "UDim");

    // UDim2
    lua_newtable(L);

    lua_pushcfunction(L, luau_UDim2_new, "UDim2.new");
    lua_setfield(L, -2, "new");
    
    lua_pushcfunction(L, luau_UDim2_fromScale, "UDim2.fromScale");
    lua_setfield(L, -2, "fromScale");

    lua_pushcfunction(L, luau_UDim2_fromOffset, "UDim2.fromOffset");
    lua_setfield(L, -2, "fromOffset");

    lua_setglobal(L, "UDim2");

    // DockWidgetPluginGuiInfo
    lua_newtable(L);

    lua_pushcfunction(L, luau_DockWidgetPluginGuiInfo_new, "DockWidgetPluginGuiInfo.new");
    lua_setfield(L, -2, "new");

    lua_setglobal(L, "DockWidgetPluginGuiInfo");

    // TweenInfo
    lua_newtable(L);

    lua_pushcfunction(L, luau_TweenInfo_new, "TweenInfo.new");
    lua_setfield(L, -2, "new");

    lua_setglobal(L, "TweenInfo");

    //global index hook
    lua_newtable(L);

    lua_pushcfunction(L, luau_globals__index, "_G.__index");
    lua_setfield(L, -2, "__index");

    lua_setmetatable(L, LUA_GLOBALSINDEX);

    // Roact globals

    lua_pushboolean(L, true);
    lua_setglobal(L, "__DEV__");

    // debug
    //lua_getglobal(L, "debug");

    //lua_pushstring(L, "loadmodule");
    //lua_pushcfunction(L, luau_require, "debug.loadmodule"); //TODO this right?
    //lua_settable(L, -3);

    //lua_setglobal(L, "debug");

    // OpenRblx internal globals
    lua_newtable(L);
    lua_setglobal(L, "__OpenRblx_require_cache");
    
}

