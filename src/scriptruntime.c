#include "scriptruntime.h"
#include "basepart.h"
#include "datamodel.h"
#include <pthread.h>

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

#define LUA_IMPL
#include "minilua.h"

//static time_t time_initialized;

static int luau_wait(lua_State *L)
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

static int luau_tick(lua_State *L)
{
    lua_pushinteger(L, time(NULL));
    return 1;
}

static int luau_Instance_FindFirstChild(lua_State *L);
static void luau_pushinstance(lua_State *L, Instance *inst);
static void luau_pushvector3(lua_State *L, Vector3 v);

static Vector3 luau_tovector3(lua_State *L, int idx)
{
    Vector3 ret = { 0 };

    lua_pushstring(L, "X");
    lua_gettable(L, idx);
    ret.x = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L, "Y");
    lua_gettable(L, idx);
    ret.y = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L, "Z");
    lua_gettable(L, idx);
    ret.z = lua_tonumber(L, -1);
    
    lua_pop(L, 1);

    return ret;
}

static int luau_vector3__mul(lua_State *L)
{
    lua_Number factor = lua_tonumber(L, 2);
    Vector3 vec = luau_tovector3(L, 1);

    Vector3 ret = (Vector3){
        vec.x * factor,
        vec.y * factor,
        vec.z * factor
    };

    luau_pushvector3(L, ret);

    return 1;
}

static void luau_pushvector3(lua_State *L, Vector3 v)
{
    lua_newtable(L);

    lua_newtable(L);

    lua_pushstring(L, "__mul");
    lua_pushcfunction(L, luau_vector3__mul);
    lua_settable(L, -3);

    lua_setmetatable(L, -2);

    lua_pushstring(L, "X");
    lua_pushnumber(L, v.x);
    lua_settable(L, -3);

    lua_pushstring(L, "Y");
    lua_pushnumber(L, v.y);
    lua_settable(L, -3);

    lua_pushstring(L, "Z");
    lua_pushnumber(L, v.z);
    lua_settable(L, -3);
}

static void luau_pushcframe(lua_State *L, CFrame cf)
{
    lua_newtable(L);

    lua_pushstring(L, "X");
    lua_pushnumber(L, cf.X);
    lua_settable(L, -3);

    lua_pushstring(L, "Y");
    lua_pushnumber(L, cf.Y);
    lua_settable(L, -3);

    lua_pushstring(L, "Z");
    lua_pushnumber(L, cf.Z);
    lua_settable(L, -3);

    lua_pushstring(L, "LookVector");
    luau_pushvector3(L, (Vector3){-cf.R02, -cf.R12, -cf.R22});
    lua_settable(L, -3);

    lua_pushstring(L, "lookVector");
    luau_pushvector3(L, (Vector3){-cf.R02, -cf.R12, -cf.R22});
    lua_settable(L, -3);

    lua_pushstring(L, "RightVector");
    luau_pushvector3(L, (Vector3){cf.R00, cf.R10, cf.R20});
    lua_settable(L, -3);

    lua_pushstring(L, "UpVector");
    luau_pushvector3(L, (Vector3){cf.R01, cf.R11, cf.R21});
    lua_settable(L, -3);
}

static int luau_Instance__index(lua_State *L)
{
    lua_pushstring(L, "__inst_ptr");
    lua_rawget(L, 1);
    Instance *inst = lua_touserdata(L, -1);
    lua_pop(L, 1);

    const char *name = lua_tostring(L, 2);
    Instance *child = Instance_FindFirstChild(inst, name, false);

    if (child)
    {
        luau_pushinstance(L, child);

        return 1;
    }

    lua_rawget(L, 1);
    if (!lua_isnil(L, -1))
        return 1;

    if (!strcmp(name, "Parent"))
    {
        luau_pushinstance(L, inst->Parent);
        return 1;
    }

    if (Instance_IsA(inst, "BasePart"))
    {
        BasePart *basepart = inst;
        if (!strcmp(name, "CFrame"))
        {
            luau_pushcframe(L, basepart->CFrame);
            return 1;
        }
        else if (!strcmp(name, "Position"))
        {
            luau_pushvector3(L, basepart->Position);
            return 1;
        }
    }

    lua_pushstring(L, TextFormat("Instance %s has no child or property %s\n", inst->ClassName, name));
    lua_error(L);

    lua_pushnil(L);
    return 1;
}

static int luau_Instance_FindFirstChild(lua_State *L)
{
    if (lua_gettop(L) < 2)
    {
        lua_pushstring(L, "Expected 2 or more arguments.");
        lua_error(L);
    }

    // Instance is at 1
    const char *name = lua_tostring(L, 2);

    lua_pushstring(L, "__inst_ptr");
    lua_rawget(L, 1);

    Instance *inst = lua_touserdata(L, -1);

    Instance *ret = Instance_FindFirstChild(inst, name, lua_gettop(L) == 4 ? lua_toboolean(L, 3) : false);

    if (!ret)
    {
        return 0;
    }

    lua_newtable(L);

    return 1;
}

static int luau_Instance_GetChildren(lua_State *L)
{
    if (lua_gettop(L) != 1)
    {
        lua_pushstring(L, "Expected 1 argument.\n");
        lua_error(L);
    }

    lua_pushstring(L, "__inst_ptr");
    lua_rawget(L, 1);

    Instance *inst = lua_touserdata(L, -1);

    lua_newtable(L);

    for (int i = 0; i < inst->childCount; i++)
    {
        lua_pushstring(L, inst->children[i]->Name);
        luau_pushinstance(L, inst->children[i]);
        lua_settable(L, -3);
    }

    return 1;
}

static int luau_Instance_Clone(lua_State *L)
{
    if (lua_gettop(L) != 1)
    {
        lua_pushstring(L, "Expected 1 arguments.\n");
        lua_error(L);
    }

    lua_pushstring(L, "__inst_ptr");
    lua_rawget(L, 1);

    Instance *inst = lua_touserdata(L, -1);

    luau_pushinstance(L, Instance_Clone(inst));

    return 1;
}

static int luau_Instance_WaitForChild(lua_State *L)
{
    int nargs = lua_gettop(L);
    if (nargs < 2)
    {
        lua_pushstring(L, "Expected at least 2 arguments.\n");
        lua_error(L);
    }

    lua_pushstring(L, "__inst_ptr");
    lua_rawget(L, 1);

    const char *childName = luaL_checkstring(L, 2);
    double timeOut = 0.0;
    if (nargs > 2)
    {
        timeOut = luaL_checknumber(L, 3);
    }

    Instance *inst = lua_touserdata(L, -1);

    luau_pushinstance(L, Instance_WaitForChild(inst, childName, timeOut));

    return 1;
}

static int luau_Instance_IsDescendantOf(lua_State *L)
{
    int nargs = lua_gettop(L);
    if (nargs != 2)
    {
        lua_pushstring(L, "Expected 2 arguments.\n");
        lua_error(L);
    }

    lua_pushstring(L, "__inst_ptr");
    lua_rawget(L, 1);

    Instance *inst = lua_touserdata(L, -1);

    lua_pushstring(L, "__inst_ptr");
    lua_rawget(L, 2);

    Instance *ancestor = lua_touserdata(L, -1);

    lua_pushboolean(L, Instance_IsDescendantOf(inst, ancestor));

    return 1;
}

static int luau_ServiceProvider_GetService(lua_State *L)
{
    lua_pushstring(L, "__inst_ptr");
    lua_gettable(L, 1);

    ServiceProvider *serviceprovider = lua_touserdata(L, -1);
    const char *serviceName = lua_tostring(L, 2);

    Instance *service = ServiceProvider_GetService(serviceprovider, serviceName);

    luau_pushinstance(L, service);

    return 1;
}

static void luau_pushinstance(lua_State *L, Instance *inst)
{
    if (!inst)
    {
        lua_pushnil(L);
        return;
    }

    lua_newtable(L);

    lua_newtable(L);

    lua_pushstring(L, "__index");
    lua_pushcfunction(L, luau_Instance__index);
    lua_settable(L, -3);

    lua_setmetatable(L, -2);

    lua_pushstring(L, "FindFirstChild");
    lua_pushcfunction(L, luau_Instance_FindFirstChild);
    lua_settable(L, -3);

    lua_pushstring(L, "findFirstChild");
    lua_pushcfunction(L, luau_Instance_FindFirstChild);
    lua_settable(L, -3);

    lua_pushstring(L, "GetChildren");
    lua_pushcfunction(L, luau_Instance_GetChildren);
    lua_settable(L, -3);

    lua_pushstring(L, "children");
    lua_pushcfunction(L, luau_Instance_GetChildren);
    lua_settable(L, -3);

    lua_pushstring(L, "Clone");
    lua_pushcfunction(L, luau_Instance_Clone);
    lua_settable(L, -3);

    lua_pushstring(L, "WaitForChild");
    lua_pushcfunction(L, luau_Instance_WaitForChild);
    lua_settable(L, -3);

    lua_pushstring(L, "IsDescendantOf");
    lua_pushcfunction(L, luau_Instance_IsDescendantOf);
    lua_settable(L, -3);
    
    lua_pushstring(L, "__inst_ptr");
    lua_pushlightuserdata(L, inst);
    lua_settable(L, -3);

    if (Instance_IsA(inst, "ServiceProvider"))
    {
        lua_pushstring(L, "GetService");
        lua_pushcfunction(L, luau_ServiceProvider_GetService);
        lua_settable(L, -3);
    }
}

static int luau_Vector3_new(lua_State *L)
{
    if (lua_gettop(L) != 3)
    {
        lua_pushstring(L, "Expected 3 arguments.\n");
        lua_error(L);
    }

    lua_Number x = lua_tonumber(L, 1);
    lua_Number y = lua_tonumber(L, 2);
    lua_Number z = lua_tonumber(L, 3);

    luau_pushvector3(L, (Vector3){x, y, z});

    return 1;
}

static void init_lua_state(lua_State *L, Script *script)
{
    luaL_openlibs(L);

    // Roblox global functions
    lua_pushcfunction(L, luau_wait);
    lua_setglobal(L, "wait");

    lua_pushcfunction(L, luau_tick);
    lua_setglobal(L, "tick");

    // Global instances
    luau_pushinstance(L, GetDataModel()->Workspace);
    lua_setglobal(L, "Workspace");

    luau_pushinstance(L, script);
    lua_setglobal(L, "script");

    luau_pushinstance(L, GetDataModel());
    lua_setglobal(L, "game");

    // Data type constructors
    lua_newtable(L);

    lua_pushstring(L, "new");
    lua_pushcfunction(L, luau_Vector3_new);
    lua_settable(L, -3);

    lua_setglobal(L, "Vector3");

    lua_newtable(L);

    lua_setglobal(L, "CFrame");
    
}

static void run_script(Instance *script, const char *source, int sourceLength, bool client)
{
    lua_State *L = luaL_newstate();

    init_lua_state(L, script);

    printf("Running script source [%s]\n", script->Name);

    if (luaL_loadbuffer(L, source, sourceLength, script->Name))
    {
        printf("Error: %s\n", lua_tostring(L, -1));
        goto end;
    }
    if (lua_pcall(L, 0, 0, 0))
    {
        printf("Error: %s\n", lua_tostring(L, -1));
        goto end;
    }
    
    printf("worked.\n");

end:
    printf("Source:  \"%s\"\n", source);
    lua_close(L);
}

typedef struct ScriptRunnerArgs {
    Instance *script;
    const char *source;
    int sourceLength;
    bool client;
    ScriptRuntime *this;
} ScriptRunnerArgs;

void *run_script_runner(void *args)
{
    ScriptRunnerArgs *srargs = args;

    run_script(srargs->script, srargs->source, srargs->sourceLength, srargs->client);

    Instance_Destroy(srargs->this);

    return NULL;
}

void ScriptRuntime_RunScript(ScriptRuntime *this, Script *script)
{
    ScriptRunnerArgs *args = malloc(sizeof(ScriptRunnerArgs));
    pthread_t thread;

    args->script = script;
    args->source = script->Source;
    args->sourceLength = script->sourceLength;
    args->client = false;
    args->this = this;

    pthread_create(&thread, NULL, run_script_runner, args);
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

