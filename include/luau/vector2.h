#ifndef _LUAU_VECTOR2_
#define _LUAU_VECTOR2_

#include <raymath.h>
#include "lua.h"

int luau_Vector2_new(lua_State *L);

void luau_pushvector2(lua_State *L, Vector2 v);
Vector2 luau_tovector2(lua_State *L, int idx);

#endif

