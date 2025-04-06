#ifndef _LUAU_SERIALIZE_
#define _LUAU_SERIALIZE_

#include "../serialize.h"
#include "lua.h"

int luau_pushserialization(lua_State *L, Serialization sz);
Serialization luau_toserialization(lua_State *L, int idx);

int sztypesiz(int t);

#endif

