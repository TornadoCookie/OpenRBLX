#include "luau/event.h"
#include "lua.h"
#include "rbxscriptsignal.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(luau)

static RBXScriptSignal *luau_toevent(lua_State *L, int i)
{
    if (i < 0) i--;
    lua_pushstring(L, "__evt_ptr");
    lua_rawget(L, i);
    RBXScriptSignal *event = lua_touserdata(L, -1);
    lua_pop(L, 1);

    return event;
}

static int luau_RBXScriptSignal_Connect(lua_State *L)
{
    RBXScriptSignal *event = luau_toevent(L, 1);

    FIXME("event %p\n", event);

    return 0;
}

int luau_pushevent(lua_State *L, RBXScriptSignal *event)
{
    lua_newtable(L);

    lua_pushcfunction(L, luau_RBXScriptSignal_Connect, "RBXScriptSignal:Connect");
    lua_setfield(L, -2, "Connect");

    lua_pushlightuserdata(L, event);
    lua_setfield(L, -2, "__evt_ptr");

    return 1;
}

