#ifndef _LUAU_BRICKCOLOR_
#define _LUAU_BRICKCOLOR_

#include "lua.h"
#include "../brickcolor.h"

int luau_BrickColor_new(lua_State *L);

BrickColor luau_tobrickcolor(lua_State *L, int idx);
void luau_pushbrickcolor(lua_State *L, BrickColor bc);

#endif

