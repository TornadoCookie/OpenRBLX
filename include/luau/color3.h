#ifndef _LUAU_COLOR3_
#define _LUAU_COLOR3_

#include "lua.h"
#include "../color3.h"

int luau_Color3_new(lua_State *L);
int luau_Color3_fromRGB(lua_State *L);
int luau_Color3_fromHex(lua_State *L);

Color3 luau_tocolor3(lua_State *L, int idx);
void luau_pushcolor3(lua_State *L, Color3 col);

#endif

