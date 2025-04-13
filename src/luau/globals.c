#include "luau/globals.h"
#include "luau/instance.h"

#include "script.h"
#include "modulescript.h"
#include "globalsettings.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(luau)

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "luacode.h"
#include "lualib.h"

#include "luau/utils.h"

// _G should not be readonly (eek)
#define luaL_sandbox(L) { \
    luaL_sandbox(L); \
    lua_setreadonly(L, LUA_GLOBALSINDEX, false); \
    lua_getglobal(L, "__OpenRblx_require_cache"); \
    lua_setreadonly(L, -1, false); \
    lua_getglobal(L, "__OpenRblx_event_ids"); \
    lua_setreadonly(L, -1, false); \
    lua_pop(L, 2); \
}

static void luaL_desandbox(lua_State *L)
{
    // set all libraries to rw
    lua_pushnil(L);
    while (lua_next(L, LUA_GLOBALSINDEX) != 0)
    {
        if (lua_istable(L, -1))
            lua_setreadonly(L, -1, false);

        lua_pop(L, 1);
    }

    // set all builtin metatables to rw
    lua_pushliteral(L, "");
    if (lua_getmetatable(L, -1))
    {
        lua_setreadonly(L, -1, false);
        lua_pop(L, 2);
    }
    else
    {
        lua_pop(L, 1);
    }

    // set globals to rw and deactivate safeenv since the env is now mutable
    lua_setreadonly(L, LUA_GLOBALSINDEX, false);
    lua_setsafeenv(L, LUA_GLOBALSINDEX, false);
}

static int luau_require_errfunc(lua_State *L)
{
    lua_Debug ar;

    for (int i = 0; lua_getinfo(L, i, "sln", &ar); i++)
    {
        printf("%d: %s", i, ar.what);

        if (strcmp(ar.what, "C"))
        {
            printf("source %s, line %d, function %s", ar.short_src, ar.currentline, ar.name);
        }
        printf("\n");
    }

    return 1;
}


#include "Luau/Bytecode.h"
static void sanitize_bytecode(uint32_t *code, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (LUAU_INSN_OP(code[i]) == LOP_FASTCALL1 && LUAU_INSN_A(code[i]) == LBF_TYPEOF)
        {
            // disable the typeof builtin by patching the bytecode
            code[i] = LOP_NOP;
        }
    }
}

static int luau_require(lua_State *L)
{
    Instance *inst = luau_toinstance(L, 1);

    //printf("require(%s)\n", inst->Name);

    if (!strcmp(inst->ClassName, "ModuleScript"))
    {
        ModuleScript *mscript = inst;

        size_t bytecodesize = 0;
        char *bytecode;
        struct lua_CompileOptions compopts = {
            .disabledBuiltins = {"typeof", NULL}
        };

        // Set the script global
        lua_getglobal(L, "script");
        Script *callingScript = luau_toinstance(L, -1);
        lua_pop(L, 1);
        luau_pushinstance(L, mscript);

        luaL_desandbox(L);
        lua_setglobal(L, "script");
        luaL_sandbox(L);

        compopts.debugLevel = 2;
        if (mscript->isBytecode)
        {
            bytecode = mscript->Source;
            bytecodesize = mscript->sourceLength;

            sanitize_bytecode(bytecode, bytecodesize);
        }
        else
        {
            bytecode = luau_compile(mscript->Source, mscript->sourceLength, &compopts, &bytecodesize);
        }

        lua_pushcfunction(L, luau_require_errfunc, "require errfunc");

        int result = luau_load(L, inst->Name, bytecode, bytecodesize, 0); 
        if (result != 0)
        {
            printf("There was a problem: %s\n", lua_tostring(L, -1));
            goto err;
        }
        if (lua_pcall(L, 0, 1, -2))
        {
            printf("Error: %s\n", lua_tostring(L, -1));
            goto err;
        }

        lua_replace(L, -2);

        luaL_desandbox(L);

        luau_pushinstance(L, callingScript);
        lua_setglobal(L, "script");

        luaL_sandbox(L);

        //printf("ran modulescript\n");
    }
    goto end;

err:
    
    if (((ModuleScript*)inst)->isBytecode)
    {
        printf("%s ", Instance_GetFullName(inst));
        disassemble(((ModuleScript*)inst)->Source, ((ModuleScript*)inst)->sourceLength);
    }
    else
    {
        printf("Source: %s\n", ((ModuleScript*)inst)->Source);
    } 

    luaL_error(L, "Requested module experienced an error while loading\n");

end:    

    //lua_pushnil(L);
    return 1;
}

int luau_require_cached(lua_State *L)
{
    Instance *inst = luau_toinstance(L, 1);

    lua_getglobal(L, "__OpenRblx_require_cache");
    
    char *fullName = Instance_GetFullName(inst);

    lua_pushstring(L, fullName);
    lua_gettable(L, -2);

    if (lua_isnil(L, -1))
    {
        //FIXME("Found cache for %s\n", fullName);
        lua_pop(L, 1);
       
        luau_require(L);

        lua_pushstring(L, fullName);
        lua_pushvalue(L, -2);
        lua_settable(L, -4);

        return 1;
    }
    else
    {
        return 1;
    }
}

int luau_pcall(lua_State* L)
{
    int nargs = lua_gettop(L);

    lua_getglobal(L, "__pcall");
    lua_insert(L, 1);
    
    lua_call(L, nargs, LUA_MULTRET);

    if (lua_gettop(L) > 1 && !lua_toboolean(L, -2))
    {
        FIXME("pcall failed: %s\n", lua_tostring(L, -1));
        
        lua_getglobal(L, "script");
        Instance *i = luau_toinstance(L, -1);
        lua_pop(L, 1);

        if (!strcmp(i->ClassName, "Script"))
        {
            Script *sc = i;
            printf("%s ", i->Name);
            
            if (sc->isBytecode)
            {
                disassemble(sc->Source, sc->sourceLength);
            }
            else
            {
                printf("Source: %s\n", sc->Source);
            }
        }
        else if (!strcmp(i->ClassName, "ModuleScript"))
        {
            ModuleScript *sc = i;
            printf("%s ", i->Name);
            
            if (sc->isBytecode)
            {
                disassemble(sc->Source, sc->sourceLength);
            }
            else
            {
                printf("Source: %s\n", sc->Source);
            }
        }
        
    }

    return lua_gettop(L);
}

int luau_xpcall(lua_State* L)
{
    int nargs = lua_gettop(L);

    printf("xpcall with %d args\n", nargs);

    lua_getglobal(L, "__xpcall");
    lua_insert(L, 1);
    
    int ts = lua_gettop(L);

    lua_call(L, nargs, LUA_MULTRET);

    int nrets = lua_gettop(L) - ts;

    if (!lua_toboolean(L, -nrets))
    {

        lua_getglobal(L, "script");
        Instance *i = luau_toinstance(L, -1);
        lua_pop(L, 1);

        if (!strcmp(i->ClassName, "Script"))
        {
            Script *sc = i;
            printf("xpcall %s ", i->Name);
            
            if (sc->isBytecode)
            {
                disassemble(sc->Source, sc->sourceLength);
            }
            else
            {
                printf("Source: %s\n", sc->Source);
            }
        }
        else if (!strcmp(i->ClassName, "ModuleScript"))
        {
            ModuleScript *sc = i;
            printf("xpcall %s ", i->Name);
            
            if (sc->isBytecode)
            {
                disassemble(sc->Source, sc->sourceLength);
            }
            else
            {
                printf("Source: %s\n", sc->Source);
            }
        }
        
    }

    return lua_gettop(L);
}

int luau_warn(lua_State *L)
{
    int nargs = lua_gettop(L);

    printf("lua warning: %d ", nargs);

    for (int i = 1; i <= nargs; i++)
    {
        printf("%s ", lua_tostring(L, i));
    }
    printf("\n");
}

int luau_typeof(lua_State *L)
{
    if (lua_istable(L, 1))
    {
        lua_getfield(L, 1, "__inst_ptr");
        if (!lua_isnil(L, -1))
        {
            FIXME("is %s\n", "instance");
            lua_pushstring(L, "Instance");
            return 1;
        }
        lua_pop(L, 1);

        //luauD_traverse(L, 1);
    }

    lua_getglobal(L, "__typeof");
    lua_insert(L, 1);
    lua_call(L, 1, 1);

    FIXME("builtin typeof returned %s\n", lua_tostring(L, -1));
    

    return 1;
}

int luau_settings(lua_State *L)
{
    luau_pushinstance(L, GetGlobalSettings());
    return 1;
}

int luau_spawn(lua_State *L)
{
    FIXME("state %p\n", L);

    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);

    lua_call(L, 2, 0);

    return 0;
}

int luau_wait(lua_State *L)
{
    lua_Number towait = 0.03;
    if (lua_gettop(L) == 1)
    {
        towait = lua_tonumber(L, 1);
    }
    lua_Number elapsed = 0.0;

    //time_t current_time = time(NULL);
    //lua_Number elapsedtotal = current_time - time_initialized;

    printf("Sleeping %lf\n", towait);
    usleep(towait*1000000);

    elapsed = towait;

    lua_pushnumber(L, elapsed);
    lua_pushnumber(L, elapsed); //elapsedTotal

    return 2;
}

int luau_tick(lua_State *L)
{
    lua_pushinteger(L, time(NULL));
    return 1;
}

int luau_task_delay(lua_State *L)
{
    float duration = lua_tonumber(L, 1);
    FIXME("duration %f\n", duration);
    
    if(duration != 0)
    {
        usleep(duration*1000000);
    }

    int nargs = lua_gettop(L) - 2;
    FIXME("%d args, %d\n", nargs, lua_isfunction(L, 2));

    lua_call(L, nargs, 0);

    return 0;
}

int luau_task_defer(lua_State *L)
{
    FIXME("state %p\n", L);
    return 0;
}

int luau_debug_loadmodule(lua_State *L)
{
    FIXME("state %p\n", L);
    return 0;
}

int luau_debug_profilebegin(lua_State *L)
{
    FIXME("state %p\n", L);
    return 0;
}

int luau_debug_profileend(lua_State *L)
{
    FIXME("state %p\n", L);
    return 0;
}

