#include "serverscriptservice.h"
#include "datamodel.h"
#include <string.h>
#include "script.h"
#include <pthread.h>
#include <time.h>

#define LUA_IMPL
#include "minilua.h"

static time_t time_initialized;

ServerScriptService *ServerScriptService_new(const char *className, Instance *parent)
{
    ServerScriptService *newInst = Instance_new("ServerScriptService", parent);

    newInst->instance.DataCost = sizeof(ServerScriptService);
    newInst = realloc(newInst, sizeof(ServerScriptService));

    time_initialized = time(NULL);

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

static int luau_wait(lua_State *L)
{
    lua_Number towait = lua_tonumber(L, -1);
    lua_Number elapsed = 0.0;

    time_t current_time = time(NULL);
    lua_Number elapsedtotal = current_time - time_initialized;

    sleep(towait);

    lua_pushnumber(L, elapsed);
    lua_pushnumber(L, elapsedtotal);

    return 2;
}

static void init_lua_state(lua_State *L)
{
    luaL_openlibs(L);

    lua_pushcfunction(L, luau_wait);
    lua_setglobal(L, "wait");
}

static void *run_script(Script *script)
{
    lua_State *L = luaL_newstate();

    init_lua_state(L);

    printf("Running script source: \"%s\"\n", script->Source);

    if (luaL_loadbuffer(L, script->Source, script->sourceLength, Instance_GetFullName(script)))
    {
        printf("Error: %s: %s\n", script->basescript.luasourcecontainer.instance.Name, lua_tostring(L, -1));
        goto end;
    }
    if (lua_pcall(L, 0, 0, 0))
    {
        printf("Error: %s: %s\n", script->basescript.luasourcecontainer.instance.Name, lua_tostring(L, -1));
    }

end:
    lua_close(L);
}

static void run_scripts(Instance *inst)
{
    if (!strcmp(inst->ClassName, "Script"))
    {
        pthread_t thread;
        pthread_create(&thread, NULL, run_script, inst);
    }

    for (int i = 0; i < inst->childCount; i++)
    {
        run_scripts(inst->children[i]);
    }
}

void ServerScriptService_RunScripts(ServerScriptService *this)
{
    run_scripts(this);
}
