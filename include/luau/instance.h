#ifndef _LUAU_INSTANCE_
#define _LUAU_INSTANCE_

#include "lua.h"
#include "../instance.h"

int luau_Instance_new(lua_State *L);

void luau_pushinstance(lua_State *L, Instance *inst);
void luau_pushvtbl(lua_State *L, Instance *inst);
Instance *luau_toinstance(lua_State *L, int idx);

#endif

