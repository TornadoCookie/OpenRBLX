#ifndef _LUAU_STATE_
#define _LUAU_STATE_

#include "plugin.h"
#include "script.h"

#include "lua.h"

void init_lua_state(lua_State *L, Script *script, bool client, bool plugin, Plugin *pluginObj);

#endif

