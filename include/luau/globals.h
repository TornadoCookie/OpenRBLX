#ifndef _LUAU_GLOBALS_
#define _LUAU_GLOBALS_

#include "lua.h"

int luau_require_cached(lua_State *L);
int luau_pcall(lua_State *L);
int luau_xpcall(lua_State *L);
int luau_warn(lua_State *L);
int luau_typeof(lua_State *L);
int luau_settings(lua_State *L);
int luau_spawn(lua_State *L);
int luau_wait(lua_State *L);
int luau_tick(lua_State *L);

int luau_task_delay(lua_State *L);
int luau_task_defer(lua_State *L);

int luau_debug_loadmodule(lua_State *L);
int luau_debug_profilebegin(lua_State *L);

#endif

