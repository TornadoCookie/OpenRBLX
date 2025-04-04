#include "scriptruntime.h"
#include "basepart.h"
#include "datamodel.h"
#include "modulescript.h"
#include "plugin.h"

#include <pthread.h>

#include <string.h>
#include <stdio.h>

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

static int luau_vector3__add(lua_State *L)
{
    Vector3 v2 = luau_tovector3(L, 2);
    Vector3 v1 = luau_tovector3(L, 1);

    Vector3 ret = Vector3Add(v1, v2);

    luau_pushvector3(L, ret);

    return 1;
}

static void luau_pushvector3(lua_State *L, Vector3 v)
{
    lua_newtable(L);

    lua_newtable(L);

    lua_pushstring(L, "__mul");
    lua_pushcfunction(L, luau_vector3__mul, "Vector3:__mul");
    lua_settable(L, -3);

    lua_pushstring(L, "__add");
    lua_pushcfunction(L, luau_vector3__add, "Vector3:__add");
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

static Instance *luau_toinstance(lua_State *L, int i)
{
    if (i < 0) i--;
    lua_pushstring(L, "__inst_ptr");
    lua_rawget(L, i);
    Instance *inst = lua_touserdata(L, -1);
    lua_pop(L, 1);

    return inst;
}

static int luau_Instance__index(lua_State *L)
{
    Instance *inst = luau_toinstance(L, 1);
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
    else if (!strcmp(name, "ClassName"))
    {
        lua_pushstring(L, inst->ClassName);
        return 1;
    }

    SerializeInstance sInst = { 0 };
    Instance_Serialize(inst, &sInst);

    for (int i = 0; i < sInst.serializationCount; i++)
    {
        if (!strcmp(name, sInst.serializations[i].name))
        {
            int type = sInst.serializations[i].type;
            void *val = sInst.serializations[i].val;

            switch (type)
            {
                case Serialize_string:
                {
                    lua_pushstring(L, *(char**)val);
                    return 1;
                } break;
                default:
                {
                    FIXME("No lua type serializer for %d\n", type);
                    luaL_error(L, "No lua type serializer for %d\n", type);
                } break;
            }
        }
    }

    free(sInst.serializations);

    FIXME("Instance %s %s has no child or property or method %s\n", inst->ClassName, inst->Name, name);
    lua_pushstring(L, TextFormat("Instance %s (name %s) has no child or property %s\n", inst->ClassName, inst->Name, name));
    lua_error(L);

    lua_pushnil(L);
    return 1;
}

static int luau_Instance__newindex(lua_State *L)
{
    Instance *inst = luau_toinstance(L, 1);
    const char *key = lua_tostring(L, 2);

    if (!inst)
    {
        printf("attempt to newindex nil with %s\n", key);
        return 0;
    }

    printf("__newindex %s %s %s\n", inst->ClassName, inst->Name, key);

    return 0;
}

static int luau_Instance_FindFirstChild(lua_State *L)
{
    if (lua_gettop(L) < 2)
    {
        lua_pushstring(L, "Expected 2 or more arguments.");
        lua_error(L);
    }

    // Instance is at 1
    Instance *inst = luau_toinstance(L, 1);
    const char *name = lua_tostring(L, 2); 

    Instance *ret = Instance_FindFirstChild(inst, name, lua_gettop(L) == 4 ? lua_toboolean(L, 3) : false);

    if (!ret)
    {
        return 0;
    }

    luau_pushinstance(L, ret);

    return 1;
}

static int luau_Instance_FindFirstAncestor(lua_State *L)
{
    if (lua_gettop(L) < 2)
    {
        lua_pushstring(L, "Expected 2 or more arguments.");
        lua_error(L);
    }

    // Instance is at 1
    Instance *inst = luau_toinstance(L, 1);
    const char *name = lua_tostring(L, 2);

    Instance *ret = Instance_FindFirstAncestor(inst, name);

    if (!ret)
    {
        return 0;
    }

    luau_pushinstance(L, ret);

    return 1;
}

static int luau_Instance_FindFirstAncestorWhichIsA(lua_State *L)
{
    if (lua_gettop(L) < 2)
    {
        lua_pushstring(L, "Expected 2 or more arguments.");
        lua_error(L);
    }

    // Instance is at 1
    Instance *inst = luau_toinstance(L, 1);
    const char *name = lua_tostring(L, 2);

    Instance *ret = Instance_FindFirstAncestorWhichIsA(inst, name);

    if (!ret)
    {
        return 0;
    }

    luau_pushinstance(L, ret);

    return 1;
}

static int luau_Instance_FindFirstChildWhichIsA(lua_State *L)
{
    if (lua_gettop(L) < 2)
    {
        lua_pushstring(L, "Expected 2 or more arguments.");
        lua_error(L);
    }

    // Instance is at 1
    Instance *inst = luau_toinstance(L, 1);
    const char *name = lua_tostring(L, 2);
    bool recursive = lua_toboolean(L, 3);

    Instance *ret = Instance_FindFirstChildWhichIsA(inst, name, recursive);

    if (!ret)
    {
        return 0;
    }

    luau_pushinstance(L, ret);

    return 1;
}

static int luau_Instance_GetChildren(lua_State *L)
{
    if (lua_gettop(L) != 1)
    {
        lua_pushstring(L, "Expected 1 argument.\n");
        lua_error(L);
    }

    Instance *inst = luau_toinstance(L, 1); 

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

    Instance *inst = luau_toinstance(L, 1); 

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

    Instance *inst = luau_toinstance(L, 1); 

    const char *childName = luaL_checkstring(L, 2);
    double timeOut = 0.0;
    if (nargs > 2)
    {
        timeOut = luaL_checknumber(L, 3);
    }

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

    Instance *inst = luau_toinstance(L, 1);
    Instance *ancestor = luau_toinstance(L, 2);

    lua_pushboolean(L, Instance_IsDescendantOf(inst, ancestor));

    return 1;
}

static int luau_ServiceProvider_GetService(lua_State *L)
{
    ServiceProvider *serviceProvider = luau_toinstance(L, 1); 
    const char *serviceName = lua_tostring(L, 2);

    Instance *service = ServiceProvider_GetService(serviceProvider, serviceName);

    if (!service)
    {
        luaL_error(L, "No such service %s\n", serviceName);
    }

    luau_pushinstance(L, service);

    return 1;
}

static int luau_DataModel_GetFastFlag(lua_State *L)
{
    DataModel *game = luau_toinstance(L, 1); 
    const char *name = lua_tostring(L, 2);

    bool flag = DataModel_GetFastFlag(game, name);

    lua_pushboolean(L, flag);
    
    return 1;
}

static int luau_DataModel_GetFastInt(lua_State *L)
{
    DataModel *game = luau_toinstance(L, 1); 
    const char *name = lua_tostring(L, 2);

    int fint = DataModel_GetFastInt(game, name);

    lua_pushinteger(L, fint);
    
    return 1;
}

static int luau_DataModel_DefineFastFlag(lua_State *L)
{
    DataModel *game = luau_toinstance(L, 1); 
    const char *name = lua_tostring(L, 2);
    bool defaultValue = lua_toboolean(L, 3);

    bool flag = DataModel_DefineFastFlag(game, name, defaultValue);

    lua_pushboolean(L, flag);
    
    return 1;
}

static int luau_DataModel_DefineFastInt(lua_State *L)
{
    DataModel *game = luau_toinstance(L, 1); 
    const char *name = lua_tostring(L, 2);
    int defaultValue = lua_tointeger(L, 3);

    int flag = DataModel_DefineFastInt(game, name, defaultValue);

    lua_pushinteger(L, flag);
    
    return 1;
}

static RBXScriptSignal *luau_toevent(lua_State *L, int i)
{
    if (i < 0) i--;
    lua_pushstring(L, "__evt_ptr");
    lua_rawget(L, i);
    RBXScriptSignal *event = lua_touserdata(L, -1);
    lua_pop(L, 1);

    return event;
}

static int luau_RBXScriptSignal_Connect(lua_State *L)
{
    RBXScriptSignal *event = luau_toevent(L, 1);

    FIXME("event %p\n", event);

    return 0;
}

static int luau_pushevent(lua_State *L, RBXScriptSignal *event)
{
    lua_newtable(L);

    lua_pushstring(L, "Connect");
    lua_pushcfunction(L, luau_RBXScriptSignal_Connect, "RBXScriptSignal:Connect");
    lua_settable(L, -3);

    lua_pushstring(L, "__evt_ptr");
    lua_pushlightuserdata(L, event);
    lua_settable(L, -3);

    return 1;
}

static int luau_Plugin_GetPluginComponent(lua_State *L)
{
    const char *name = lua_tostring(L, 2);

    if (!strcmp(name, "PlaceManager"))
    {
        lua_newtable(L);
        
        RBXScriptSignal *PlaceDocPanelShown = RBXScriptSignal_new();
        lua_pushstring(L, "PlaceDocPanelShown");
        luau_pushevent(L, PlaceDocPanelShown);
        lua_settable(L, -3);
        
        return 1;
    }

    FIXME("name %s\n", name);

    return 0;
}

static int luau_Plugin_IsPlaceDocumentOpen(lua_State *L)
{
    Plugin *plugin = luau_toinstance(L, 1);

    FIXME("plugin %p\n", plugin);

    lua_pushboolean(L, true);

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

    lua_pushstring(L, "FindFirstChild");
    lua_pushcfunction(L, luau_Instance_FindFirstChild, "Instance:FindFirstChild");
    lua_settable(L, -3);

    lua_pushstring(L, "findFirstChild");
    lua_pushcfunction(L, luau_Instance_FindFirstChild, "Instance:findFirstChild");
    lua_settable(L, -3);

    lua_pushstring(L, "FindFirstAncestor");
    lua_pushcfunction(L, luau_Instance_FindFirstAncestor, "Instance:FindFirstAncestor");
    lua_settable(L, -3);

    lua_pushstring(L, "FindFirstAncestorWhichIsA");
    lua_pushcfunction(L, luau_Instance_FindFirstAncestorWhichIsA, "Instance:FindFirstAncestorWhichIsA");
    lua_settable(L, -3);

    lua_pushstring(L, "FindFirstChildWhichIsA");
    lua_pushcfunction(L, luau_Instance_FindFirstChildWhichIsA, "Instance:FindFirstChildWhichIsA");
    lua_settable(L, -3);

    lua_pushstring(L, "GetChildren");
    lua_pushcfunction(L, luau_Instance_GetChildren, "Instance:GetChildren");
    lua_settable(L, -3);

    lua_pushstring(L, "children");
    lua_pushcfunction(L, luau_Instance_GetChildren, "Instance:children");
    lua_settable(L, -3);

    lua_pushstring(L, "Clone");
    lua_pushcfunction(L, luau_Instance_Clone, "Instance:Clone");
    lua_settable(L, -3);

    lua_pushstring(L, "WaitForChild");
    lua_pushcfunction(L, luau_Instance_WaitForChild, "Instance:WaitForChild");
    lua_settable(L, -3);

    lua_pushstring(L, "IsDescendantOf");
    lua_pushcfunction(L, luau_Instance_IsDescendantOf, "Instance:IsDescendantOf");
    lua_settable(L, -3);

    lua_pushstring(L, "__inst_ptr");
    lua_pushlightuserdata(L, inst);
    lua_settable(L, -3);

    if (Instance_IsA(inst, "ServiceProvider"))
    {
        lua_pushstring(L, "GetService");
        lua_pushcfunction(L, luau_ServiceProvider_GetService, "ServiceProvider:GetService");
        lua_settable(L, -3);

        if (Instance_IsA(inst, "DataModel"))
        {
            lua_pushstring(L, "GetFastFlag");
            lua_pushcfunction(L, luau_DataModel_GetFastFlag, "DataModel:GetFastFlag");
            lua_settable(L, -3);

            lua_pushstring(L, "DefineFastFlag");
            lua_pushcfunction(L, luau_DataModel_DefineFastFlag, "DataModel:DefineFastFlag");
            lua_settable(L, -3);

            lua_pushcfunction(L, luau_DataModel_GetFastInt, "DataModel:GetFastInt");
            lua_setfield(L, -2, "GetFastInt");

            lua_pushcfunction(L, luau_DataModel_DefineFastInt, "DataModel:DefineFastInt");
            lua_setfield(L, -2, "DefineFastInt");
        }
    }

    if (!strcmp(inst->ClassName, "Plugin"))
    {
        lua_pushstring(L, "GetPluginComponent");
        lua_pushcfunction(L, luau_Plugin_GetPluginComponent, "Plugin:GetPluginComponent");
        lua_settable(L, -3);

        lua_pushstring(L, "IsPlaceDocumentOpen");
        lua_pushcfunction(L, luau_Plugin_IsPlaceDocumentOpen, "Plugin:IsPlaceDocumentOpen");
        lua_settable(L, -3);
    }

    lua_newtable(L);

    lua_pushstring(L, "__index");
    lua_pushcfunction(L, luau_Instance__index, "Instance:__index");
    lua_settable(L, -3);

    lua_pushstring(L, "__newindex");
    lua_pushcfunction(L, luau_Instance__newindex, "Instance:__newindex");
    lua_settable(L, -3);

    lua_setmetatable(L, -2);
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

void disassemble(uint8_t *data, int dataSize);

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

static int luau_require(lua_State *L)
{
    Instance *inst = luau_toinstance(L, 1);

    //printf("require(%s)\n", inst->Name);

    if (!strcmp(inst->ClassName, "ModuleScript"))
    {
        ModuleScript *mscript = inst;

        size_t bytecodesize = 0;
        char *bytecode;
        struct lua_CompileOptions compopts = { 0 };

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
        }
        else
        {
            bytecode = luau_compile(mscript->Source, mscript->sourceLength, &compopts, &bytecodesize);
        }

        int result = luau_load(L, inst->Name, bytecode, bytecodesize, 0); 
        if (result != 0)
        {
            printf("There was a problem: %s\n", lua_tostring(L, -1));
            goto err;
        }
        if (lua_pcall(L, 0, 1, 0))
        {
            printf("Requested module experienced an error while loading: Error: %s\n", lua_tostring(L, -1));
            goto err;
        } 

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

        printf("%s ", inst->Name);
        disassemble(((ModuleScript*)inst)->Source, ((ModuleScript*)inst)->sourceLength);
    }
    else
    {
        printf("Source: %s\n", ((ModuleScript*)inst)->Source);
    }

end:
    

    //lua_pushnil(L);
    return 1;
}

static int luau_require_cached(lua_State *L)
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

static int luau_Enums_GetEnums(lua_State *L)
{

#define enum_start(n) lua_pushstring(L, #n); lua_newtable(L);
#define enum_val(n, v) lua_pushstring(L, #n); lua_pushinteger(L, v); lua_settable(L, -3);
#define enum_end() lua_settable(L, -3);
    lua_newtable(L);

    enum_start(ZIndexBehavior);
    enum_val(Global, 0);
    enum_val(Sibling, 1);
    enum_end();

    enum_start(InitialDockState);
    enum_val(Top, 0);
    enum_val(Bottom, 1);
    enum_val(Left, 2);
    enum_val(Right, 3);
    enum_val(Float, 4);
    enum_end();

#undef enum_end
#undef enum_val
#undef enum_start

    return 1;
}

static int luau_Enums__index(lua_State *L)
{
    const char *key = lua_tostring(L, 2);

    luau_Enums_GetEnums(L);
    lua_pushstring(L, key);
    lua_gettable(L, -2);

    if (lua_isnil(L, -1))
    {
        FIXME("key %s\n", key);
    }

    return 1;
}

static int luau_DockWidgetPluginGuiInfo_new(lua_State *L)
{
    int InitialDockState = lua_tointeger(L, 1);
    bool InitialEnabled = lua_toboolean(L, 2);
    bool InitialEnabledShouldOverrideRestore = lua_toboolean(L, 3);
    float FloatingXSize = lua_tonumber(L, 4);
    float FloatingYSize = lua_tonumber(L, 5);
    float MinWidth = lua_tonumber(L, 6);
    float MinHeight = lua_tonumber(L, 7);

    lua_newtable(L);

#define set(n, T) lua_pushstring(L, #n); lua_push##T(L, n); lua_settable(L, -3);

    set(InitialDockState, integer);
    set(InitialEnabled, boolean);
    set(InitialEnabledShouldOverrideRestore, boolean);
    set(FloatingXSize, number);
    set(FloatingYSize, number);
    set(MinWidth, number);
    set(MinHeight, number);

#undef set

    return 1;
}

static int luau_pcall(lua_State* L)
{
    int nargs = lua_gettop(L);

    printf("pcall with %d args\n", nargs);

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

static int luau_xpcall(lua_State* L)
{
    int nargs = lua_gettop(L);

    printf("pcall with %d args\n", nargs);

    lua_getglobal(L, "__xpcall");
    lua_insert(L, 1);
    
    lua_call(L, nargs, LUA_MULTRET);

    if (!lua_toboolean(L, -2))
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

static int luau_globals__index(lua_State *L)
{
    char *name = lua_tostring(L, 2);

    FIXME("global name %s\n", name);
}

static int luau_warn(lua_State *L)
{
    int nargs = lua_gettop(L);

    printf("lua warning: ");

    for (int i = 1; i < nargs; i++)
    {
        printf("%s ", lua_tostring(L, i));
    }
    printf("\n");
}

static int luau_Instance_new(lua_State *L)
{
    const char *className = lua_tostring(L, 1);
    Instance *parent = luau_toinstance(L, 2);

    Instance *newInst = Instance_dynNew(className, parent);

    luau_pushinstance(L, newInst);
    return 1;
}

static void init_lua_state(lua_State *L, Script *script, bool client, bool plugin, Plugin *pluginObj)
{
    //luaL_openlibs(L);
    
    // Lua global functions
    lua_pushcfunction(L, luau_require_cached, "require");
    lua_setglobal(L, "require");

    lua_getglobal(L, "pcall");
    lua_setglobal(L, "__pcall");

    lua_getglobal(L, "xpcall");
    lua_setglobal(L, "__xpcall");

    lua_pushcfunction(L, luau_pcall, "pcall");
    lua_setglobal(L, "pcall");

    lua_pushcfunction(L, luau_xpcall, "xpcall");
    lua_setglobal(L, "xpcall");

    // Roblox global functions
    lua_pushcfunction(L, luau_wait, "wait");
    lua_setglobal(L, "wait");

    lua_pushcfunction(L, luau_tick, "tick");
    lua_setglobal(L, "tick");

    lua_pushcfunction(L, luau_warn, "warn");
    lua_setglobal(L, "warn");

    // Global instances
    luau_pushinstance(L, GetDataModel()->Workspace);
    lua_setglobal(L, "Workspace");

    luau_pushinstance(L, script);
    lua_setglobal(L, "script");

    luau_pushinstance(L, GetDataModel());
    lua_setglobal(L, "game");

    if (plugin)
    {
        luau_pushinstance(L, pluginObj);
        lua_setglobal(L, "plugin");
    }
    
    // task library
    lua_newtable(L);
    
    lua_setglobal(L, "task");

    // Global data types
    lua_newtable(L);

    lua_pushstring(L, "GetEnums");
    lua_pushcfunction(L, luau_Enums_GetEnums, "Enums:GetEnums");
    lua_settable(L, -3);

    lua_newtable(L);
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, luau_Enums__index, "Enums:__index");
    lua_settable(L, -3);
    
    lua_setmetatable(L, -2);

    lua_setglobal(L, "Enum");

    // Data type constructors
    // Vector3
    lua_newtable(L);

    lua_pushstring(L, "new");
    lua_pushcfunction(L, luau_Vector3_new, "Vector3.new");
    lua_settable(L, -3);

    lua_setglobal(L, "Vector3");

    // CFrame
    lua_newtable(L);

    lua_setglobal(L, "CFrame");

    // Instance
    lua_newtable(L);

    lua_pushcfunction(L, luau_Instance_new, "Instance.new");
    lua_setfield(L, -2, "new");

    lua_setglobal(L, "Instance");

    // DockWidgetPluginGuiInfo
    lua_newtable(L);

    lua_pushstring(L, "new");
    lua_pushcfunction(L, luau_DockWidgetPluginGuiInfo_new, "DockWidgetPluginGuiInfo.new");
    lua_settable(L, -3);

    lua_setglobal(L, "DockWidgetPluginGuiInfo");

    //global index hook
    lua_newtable(L);

    lua_pushstring(L, "__index");
    lua_pushcfunction(L, luau_globals__index, "_G.__index");
    lua_settable(L, -3);

    lua_setmetatable(L, LUA_GLOBALSINDEX);

    // Roact globals

    lua_pushboolean(L, true);
    lua_setglobal(L, "__DEV__");

    // debug
    //lua_getglobal(L, "debug");

    //lua_pushstring(L, "loadmodule");
    //lua_pushcfunction(L, luau_require, "debug.loadmodule"); //TODO this right?
    //lua_settable(L, -3);

    //lua_setglobal(L, "debug");

    // OpenRblx internal globals
    lua_newtable(L);
    lua_setglobal(L, "__OpenRblx_require_cache");
    
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
    if (lua_pcall(L, 0, 0, 0))
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

