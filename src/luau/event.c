#include "luau/event.h"
#include "lua.h"
#include "rbxscriptsignal.h"
#include <stdlib.h>

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

typedef struct luau_event_ud {
    lua_State *L;
    int eventid;
} luau_event_ud;

static void luau_event_func(void *arg, void *ud)
{
    luau_event_ud *info = ud;
    lua_State *L = info->L;

    lua_getglobal(L, "__OpenRblx_event_ids");
    lua_pushinteger(L, info->eventid);
    lua_gettable(L, -2);

    lua_call(L, 0, 0);
}

static int luau_RBXScriptSignal_Connect(lua_State *L)
{
    RBXScriptSignal *event = luau_toevent(L, 1);
    static int eventid = 0;
    luau_event_ud *ud;

    ud = malloc(sizeof(*ud));

    ud->L = L;
    ud->eventid = eventid;

    lua_getglobal(L, "__OpenRblx_event_ids");
    lua_pushinteger(L, eventid);
    lua_pushvalue(L, 2);
    lua_settable(L, -3);

    RBXScriptSignal_Connect(event, luau_event_func, ud);

    eventid++;

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

