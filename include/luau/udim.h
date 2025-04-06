#ifndef _LUAU_UDIM_
#define _LUAU_UDIM_

#include "lua.h"
#include "../udim.h"

int luau_UDim_new(lua_State *L);

UDim luau_toudim(lua_State *L, int idx);
void luau_pushudim(lua_State *L, UDim udim);

#endif

