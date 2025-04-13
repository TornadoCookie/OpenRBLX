#ifndef _LUAU_UTILS_
#define _LUAU_UTILS_

#define luau_expectargs(L, n) { \
    for (int i = 1; i < n; i++) \
        if (lua_gettop(L) < i || lua_isnil(L, i)) luaL_error(L, "%s: argument %d missing or nil", __FUNCTION__, i); }

#define luau_expectargtype(L, idx, t) { \
    if (!lua_is##t(L, idx)) luaL_error(L, "%s: argument %d: expected " #t ", got %s", __FUNCTION__, idx, lua_typename(L, lua_type(L, idx))); }

#define luauD_traverse(L, t) \
{\
/* table is in the stack at index 't' */ \
     lua_pushnil(L);  /* first key */ \
     while (lua_next(L, t) != 0) { \
       /* uses 'key' (at index -2) and 'value' (at index -1) */ \
       if (lua_type(L, 2) == LUA_TSTRING) \
       { \
           FIXME("[\"%s\"] - %s\n", lua_tostring(L, -2), lua_typename(L, lua_type(L, 1))); \
       } \
       else \
       { \
       FIXME("%s - %s\n", \
              lua_typename(L, lua_type(L, -2)), \
              lua_typename(L, lua_type(L, -1))); \
       } \
       /* removes 'value'; keeps 'key' for next iteration */ \
       lua_pop(L, 1); \
     } \
} 

#endif

