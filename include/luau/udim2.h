#ifndef _LUAU_UDIM2_
#define _LUAU_UDIM2_

#include "lua.h"
#include "../udim2.h"

int luau_UDim2_new(lua_State *L);
int luau_UDim2_fromOffset(lua_State *L);
int luau_UDim2_fromScale(lua_State *L);

void luau_pushudim2(lua_State *L, UDim2 udim2);
UDim2 luau_toudim2(lua_State *L, int idx);

#endif

