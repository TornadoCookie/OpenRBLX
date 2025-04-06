#include "scriptruntime.h"
#include "basepart.h"
#include "datamodel.h"
#include "modulescript.h"
#include "plugin.h"
#include "udim2.h"
#include "globalsettings.h"

#include <pthread.h>

#include <string.h>
#include <stdio.h>

#include "luau/state.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(scriptruntime);

ScriptRuntime *ScriptRuntime_new(const char *className, Instance *parent)
{
    ScriptRuntime *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

#include "rlw32compat.h"
#include "lua.h"
#include "luacode.h"
#include "lualib.h"

// _G should not be readonly (eek)
#define luaL_sandbox(L) { \
    luaL_sandbox(L); \
    lua_setreadonly(L, LUA_GLOBALSINDEX, false); \
    lua_getglobal(L, "__OpenRblx_require_cache"); \
    lua_setreadonly(L, -1, false); \
    lua_pop(L, 1); \
}

static void run_script(Script *script, const char *source, int sourceLength, bool client, bool plugin, Plugin *pluginObj)
{
    lua_State *L = luaL_newstate();

    luaL_openlibs(L);
    init_lua_state(L, script, client, plugin, pluginObj);

    printf("Running script source [%s]\n", ((Instance*)script)->Name);

    size_t bytecodesize = 0;
    char *bytecode;
    if (script->isBytecode)
    {
        bytecode = source;
        bytecodesize = sourceLength;

        SaveFileData("bytecode", source, sourceLength);
    }
    else
    {
        struct lua_CompileOptions compopts = { 0 };

        compopts.debugLevel = 2;

        bytecode = luau_compile(source, sourceLength, &compopts, &bytecodesize);
    }
    int result = luau_load(L, ((Instance*)script)->Name, bytecode, bytecodesize, 0);
    
    if (!script->isBytecode)
        free(bytecode);
    
    if (result != 0)
    {
        printf("There was a problem: %s\n", lua_tostring(L, -1));
        goto end;
    }
    luaL_sandbox(L);
    //lua_callbacks(L)->debugstep = scrt_debugstep;
    //lua_singlestep(L, 1);
    if (lua_pcall(L, 0, LUA_MULTRET, 0))
    {
        printf("Error: %s\n", lua_tostring(L, -1));
        goto end;
    }
    
    printf("worked.\n");

end:
    printf("Source:  ");

    if (script->isBytecode)
    {
        printf("[%d bytes of bytecode]\n", sourceLength);
        disassemble(source, sourceLength);
    }
    else
    {
        printf("\"%s\"\n", source);
        SaveFileData("bytecode", source, sourceLength);
    }
    lua_close(L);
}

typedef struct ScriptRunnerArgs {
    Script *script;
    const char *source;
    int sourceLength;
    bool client;
    bool plugin;
    Plugin *pluginObj;
    ScriptRuntime *this;
} ScriptRunnerArgs;

void *run_script_runner(void *args)
{
    ScriptRunnerArgs *srargs = args;

    run_script(srargs->script, srargs->source, srargs->sourceLength, srargs->client, srargs->plugin, srargs->pluginObj);

    Instance_Destroy(srargs->this);

    return NULL;
}

static void ScriptRuntime_RunScriptInternal(ScriptRuntime *this, Script *script, bool plugin, Plugin *pluginObj)
{
    ScriptRunnerArgs *args = malloc(sizeof(ScriptRunnerArgs));
    pthread_t thread;

    args->script = script;
    args->source = script->Source;
    args->sourceLength = script->sourceLength;
    args->client = false;
    args->plugin = plugin;
    args->pluginObj = pluginObj;
    args->this = this;

    pthread_create(&thread, NULL, run_script_runner, args);
}

void ScriptRuntime_RunScript(ScriptRuntime *this, Script *script)
{
    ScriptRuntime_RunScriptInternal(this, script, false, NULL);
}

void ScriptRuntime_RunPluginScript(ScriptRuntime *this, Script *script, Instance *plugin)
{
    ScriptRuntime_RunScriptInternal(this, script, true, plugin);
}

void ScriptRuntime_RunLocalScript(ScriptRuntime *this, LocalScript *localScript)
{
    ScriptRunnerArgs *args = malloc(sizeof(ScriptRunnerArgs));
    pthread_t thread;

    args->script = localScript;
    args->source = localScript->script.Source;
    args->sourceLength = localScript->script.sourceLength;
    args->client = true;
    args->this = this;

    pthread_create(&thread, NULL, run_script_runner, args);
}

