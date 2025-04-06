#ifndef _LUAU_EVENT_
#define _LUAU_EVENT_

#include "lua.h"
#include "rbxscriptsignal.h"

int luau_pushevent(lua_State *L, RBXScriptSignal *event);

#endif

