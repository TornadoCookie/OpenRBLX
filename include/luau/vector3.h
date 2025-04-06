#ifndef _LUAU_VECTOR3_
#define _LUAU_VECTOR3_

#include <raymath.h>
#include "lua.h"

int luau_Vector3_new(lua_State *L);

void luau_pushvector3(lua_State *L, Vector3 v);
Vector3 luau_tovector3(lua_State *L, int idx);

#endif

