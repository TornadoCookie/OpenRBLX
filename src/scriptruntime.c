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
                    luaL_error(L, "No lua type serializer for %d\n", type);
                } break;
            }
        }
    }

    free(sInst.serializations);

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

static int luau_DataModel_DefineFastFlag(lua_State *L)
{
    DataModel *game = luau_toinstance(L, 1); 
    const char *name = lua_tostring(L, 2);
    bool defaultValue = lua_toboolean(L, 3);

    bool flag = DataModel_DefineFastFlag(game, name, defaultValue);

    lua_pushboolean(L, flag);
    
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
        }
    }

    if (!strcmp(inst->ClassName, "Plugin"))
    {
        lua_pushstring(L, "GetPluginComponent");
        lua_pushcfunction(L, luau_Plugin_GetPluginComponent, "Plugin:GetPluginComponent");
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

static void disassemble(uint8_t *data, int dataSize);

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

        luau_pushinstance(L, callingScript);

        luaL_desandbox(L);
        lua_setglobal(L, "script");
        luaL_sandbox(L);

        //printf("ran modulescript\n");
    }
    goto end;

err:
    if (((ModuleScript*)inst)->isBytecode)
    {
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

static void init_lua_state(lua_State *L, Script *script, bool client, bool plugin)
{
    //luaL_openlibs(L);
    
    // Lua global functions
    lua_pushcfunction(L, luau_require, "require");
    lua_setglobal(L, "require");

    // Roblox global functions
    lua_pushcfunction(L, luau_wait, "wait");
    lua_setglobal(L, "wait");

    lua_pushcfunction(L, luau_tick, "tick");
    lua_setglobal(L, "tick");

    // Global instances
    luau_pushinstance(L, GetDataModel()->Workspace);
    lua_setglobal(L, "Workspace");

    luau_pushinstance(L, script);
    lua_setglobal(L, "script");

    luau_pushinstance(L, GetDataModel());
    lua_setglobal(L, "game");

    if (plugin)
    {
        Plugin *plugin = Plugin_new("Plugin", NULL);

        luau_pushinstance(L, plugin);
        lua_setglobal(L, "plugin");
    }

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

    // DockWidgetPluginGuiInfo
    lua_newtable(L);

    lua_pushstring(L, "new");
    lua_pushcfunction(L, luau_DockWidgetPluginGuiInfo_new, "DockWidgetPluginGuiInfo.new");
    lua_settable(L, -3);

    lua_setglobal(L, "DockWidgetPluginGuiInfo");
    
}

static void scrt_debugstep(lua_State *L, lua_Debug *ar)
{
    if (ar->what)
    {
        printf("%s:%d\n", ar->what, ar->currentline);
    }
}

static void scrt_interrupt(lua_State *L, int gc)
{
    printf("gc %d\n", gc);
}

#include "Luau/Bytecode.h"

static unsigned int readVarInt(char **pData)
{
    unsigned result = 0, shift = 0;
    uint8_t byte;
    char *data = *pData;

    do {
        byte = *(uint8_t*)data;
        data++;
        result |= byte & 127 << shift;
        shift += 7;
    } while (byte & 128);

    *pData = data;

    return result;
}

static void remapUserdataTypes(char *data, size_t size, uint32_t count)
{
    uint32_t typeSize = readVarInt(&data);
    uint32_t upvalCount = readVarInt(&data);
    uint32_t localCount = readVarInt(&data);

    printf("remap Userdata Types:\n");
    printf("Type size: %d\n", typeSize);
    if (typeSize)
    {
        printf("TODO typeSize\n");
    }

    printf("upval Count: %d\n", upvalCount);
    if (upvalCount)
    {
        uint8_t *types = data;
        for (int i = 0; i < upvalCount; i++)
        {
            uint32_t index = ((uint32_t)types[i]) - LBC_TYPE_TAGGED_USERDATA_BASE;

            printf("upval %d: %d\n", i, index);
        }
        data += upvalCount;
    }

    printf("local Count: %d\n", localCount);
    if (localCount)
    {
        for (int i = 0; i < localCount; i++)
        {
            uint32_t index = *data;
            index -= LBC_TYPE_TAGGED_USERDATA_BASE;
            printf("local %d: %d\n", i, index);

            data += 2;
            readVarInt(&data);
            readVarInt(&data);

        }
    }
}

static char *readString(uint8_t **data, char **strings)
{
    int id = readVarInt(data);
    printf("rs %d\n", id);
    if (id == 0) return NULL;
    return strings[id - 1];
}

// sign extend 24 to 32 bits
int32_t se24_32(uint32_t x)
{
    if (x >> 24)
    {
        x |= 0xff000000;
    }
    return x;
}

typedef struct LuauConstant {
    int type;
    void *val;
    int constantCount;
} LuauConstant;

static void print_constant(LuauConstant *constants, unsigned int i, bool putquotes)
{
    if (constants->constantCount < i)
    {
        printf("KInvalid_%d", i);
        return;
    }

    if (constants[i].type == LBC_CONSTANT_STRING)
    {
        if (putquotes)
        {
            printf("\"%s\"", constants[i].val);
        }
        else
        {
            printf("%s", constants[i].val);
        }
    }
    else
    {
        printf("K%d", constants[i].type, i);
    }
}

// Adapted from Luau/Bytecode.h documentation
static void disassemblecode(uint32_t *code, int codeSize, LuauConstant *constants) 
{
    for (int i = 0; i < codeSize; i++)
    {
        printf("%05x:", i);
        uint8_t opcode = code[i] & 0xff;
        uint8_t a = (code[i] >>  8) & 0xff;
        uint8_t b = (code[i] >> 16) & 0xff;
        uint8_t c = (code[i] >> 24) & 0xff;
        int16_t d = (code[i] >> 16) & 0xffff;
        int32_t e = se24_32(code[i] >> 8);
        uint32_t aux = 0;

        if (i != codeSize - 1)
        {
            aux = code[i+1];
        }

        switch (opcode)
        {
            case LOP_NOP: // 0
            {
                printf("NOP\n");
            } break;
            case LOP_BREAK: // 1
            {
                printf("BREAK\n");
            } break;
            case LOP_LOADNIL: // 2
            {
                printf("R%d = nil\n", a);
            } break;
            case LOP_LOADB: // 3
            {
                printf("R%d = %s", a, b?"true":"false");
                if (c)
                {
                    printf(", goto %#x\n", i+c);
                }
                else
                {
                    printf("\n");
                }
            } break;
            case LOP_LOADN: // 4
            {
                printf("R%d = %d\n", a, d);
            } break;
            case LOP_LOADK: // 5
            {
                printf("R%d = ", a);
                print_constant(constants, d, true);
                printf("\n");
            } break;
            case LOP_MOVE: // 6
            {
                printf("R%d = R%d\n", a, b);
            } break;
            case LOP_GETGLOBAL: // 7
            {
                printf("GETGLOBAL %d %d %#x\n", a, c, aux);
                i++;
            } break;
            case LOP_SETGLOBAL: // 8
            {
                printf("SETGLOBAL %d %d %#x\n", a, c, aux);
                i++;
            } break;
            case LOP_GETUPVAL: // 9
            {
                printf("R%d = upval %d\n", a, b);
            } break;
            case LOP_SETUPVAL: // 10
            {
                printf("SETUPVAL %d %d\n", a, b);
            } break;
            case LOP_CLOSEUPVALS: // 11
            {
                printf("CLOSEUPVALS %d", a);
            } break;
            case LOP_GETIMPORT: // 12
            {
                int pcount = aux >> 30;
                int p3 = aux & 1023;
                int p2 = (aux >> 10) & 1023;
                int p1 = (aux >> 20) & 1023;
        
                printf("R%d = ", a);

                if (pcount > 0)
                {
                    print_constant(constants, p1, false);
                    if (pcount > 1)
                    {
                        printf(".");
                        print_constant(constants, p2, false);
                        if (pcount > 2)
                        {
                            printf(".");
                            print_constant(constants, p3, false);
                        }
                    }
                }
                printf("\n");

                i++;
            } break;
            //LOP_GETTABLE   13
            case LOP_SETTABLE: // 14
            {
                printf("R%d[R%d] = R%d\n", b, c, a);
            } break;
            case LOP_GETTABLEKS: // 15
            {
                printf("R%d = R%d[", a, b, aux);
                print_constant(constants, aux, true);
                printf("]\n");
                i++;
            } break;
            case LOP_SETTABLEKS: // 16
            {
                printf("R%d[", b);
                print_constant(constants, aux, true);
                printf("] = R%d\n", a);
                i++;
            } break;
            //LOP_GETTABLEN  17
            //LOP_SETTABLEN  18
            case LOP_NEWCLOSURE: // 19
            {
                printf("R%d = new closure from %d\n", a, d);
            } break;
            case LOP_NAMECALL: // 20
            {
                printf("R%d = R%d, R%d = R%d[", a+1, b, a, b, aux);
                print_constant(constants, aux, true);
                printf("]\n");
                i++;
            } break;
            case LOP_CALL: // 21
            {
                if (c == 0)
                {
                    printf("... = ");
                }
                else if (c == 2)
                {
                    printf("R%d = ", a);
                }
                printf("R%d(", a);
                if (b == 0)
                {
                    printf("...");
                }
                for (int j = a+1; j < a+b; j++)
                {
                    printf("R%d", j);
                    if (j != a+b-1)
                    {
                        printf(", ");
                    }
                }
                printf(") -> %d rets\n", c-1);
            } break;
            case LOP_RETURN: // 22
            {
                if (b == 1)
                {
                    printf("return\n");
                }
                else if (b > 0)
                {
                    printf("return ");
                    for (int j = a; j < a+b-1; j++)
                    {
                        printf("R%d", j);
                        if (j != a+b-2) printf(", ");
                    }
                    printf("\n");
                }
                else
                {
                    printf("RETURN %d %d\n", a, b);
                }
            } break;
            //LOP_JUMP       23
            //LOP_JUMPBACK   24
            case LOP_JUMPIF: // 25
            {
                printf("if R%d: go to %#x\n", a, i+d+1);
            } break;
            case LOP_JUMPIFNOT: // 26
            {
                printf("if not R%d: go to %#x\n", a, i+d+1);
            } break;
            case LOP_JUMPIFEQ: // 27
            {
                printf("if R%d == R%d: go to %#x\n", a, aux, i+d+1);
                i++;
            } break;
            //LOP_JUMPIFLE   28
            //LOP_JUMPIFLT   29
            //LOP_JUMPIFNOTEQ 30
            //LOP_JUMPIFNOTLE 31
            //LOP_JUMPIFNOTLT 32
            //LOP_ADD        33
            //LOP_SUB        34
            //LOP_MUL        35
            //LOP_DIV        36
            //LOP_MOD        37
            //LOP_POW        38
            //LOP_ADDK       39
            //LOP_SUBK       40
            //LOP_MULK       41
            //LOP_DIVK       42
            //LOP_MODK       43
            //LOP_POWK       44
            //LOP_AND        45
            //LOP_OR         46
            //LOP_ANDK       47
            //LOP_ORK        48
            //LOP_CONCAT     49
            //LOP_NOT        50
            //LOP_MINUS      51
            //LOP_LENGTH     52
            case LOP_NEWTABLE: // 53
            {
                printf("R%d = {}, arrsz=%d, tsz=%d\n", a, aux, b);
                i++;
            } break;
            case LOP_DUPTABLE: // 54
            {
                printf("R%d = ", a);
                print_constant(constants, d, true);
                printf("(duptable)\n");
            } break;
            //LOP_SETLIST    55
            //LOP_FORNPREP   56
            //LOP_FORNLOOP   57
            case LOP_FORGLOOP: // 58
            {
                printf("(FORGLOOP) ");
                for (int j = a+3; j < a+3+(aux&0xff); j++)
                {
                    printf("R%d", j);
                    if (j != a+2+(aux&0xff))
                        printf(", ");
                }
                printf(" = R%d(R%d, R%d), goto %#x\n", a, a+1, a+2, i+d+1);
                i++;
            } break;
            //LOP_FORGPREP_INEXT 59
            //LOP_FASTCALL3  60
            case LOP_FORGPREP_NEXT: // 61
            {
                printf("R%d = nil, R%d = nil, go to FORGLOOP\n", a+3, a+4);
            } break;
            //LOP_NATIVECALL 62
            case LOP_GETVARARGS: // 63
            {
                printf("GETVARARGS %d %d\n", a, b);
            } break;
            case LOP_DUPCLOSURE: // 64
            {
                printf("R%d = K%d (closure)\n", a, d);
            } break;
            case LOP_PREPVARARGS: // 65
            {
                printf("PREPVARARGS %d\n", a);
            } break;
            //LOP_LOADKX     66
            //LOP_JUMPX      67
            //LOP_FASTCALL   68
            //LOP_COVERAGE   69
            case LOP_CAPTURE: // 70
            {
                printf("CAPTURE %d %d\n", a, b);
            } break;
            case LOP_SUBRK: // 71
            {
                printf("SUBRK %d %d %d\n", a, b, c);
            } break;
            //LOP_DIVRK       72
            //LOP_FASTCALL1   73
            //LOP_FASTCALL2   74
            //LOP_FASTCALL2K  75
            //LOP_FORGPREP    76
            //LOP_JUMPXEQKNIL 77
            //LOP_JUMPXEQKB   78
            //LOP_JUMPXEQKN   79
            case LOP_JUMPXEQKS: // 80
            {
                printf("if R%d ", a);
                if (aux & 0xffff0000)
                {
                    printf("~= ");
                }
                else
                {
                    printf("== ");
                }
                print_constant(constants, aux & 0x0000ffff, true);
                printf(": go to %#x\n", i+d);
                i++;
            } break;
            default:
            {
                printf("UNKNOWN (%d)\n", opcode);
            } break;
        }
    }
}

// Adapted from luau_load() at luau/VM/src/lvmload.cpp:245
static void disassemble(uint8_t *data, int dataSize)
{
    printf("Disassembly:\n");

    uint8_t version = *(uint8_t*)data;
    data++;

    printf("Version: %d\n", version);

    uint8_t typesversion = 0;

    if (version >= 4)
    {
        typesversion = *(uint8_t*)data;
        data++;
    }

    printf("Types version: %d\n", typesversion);

    unsigned stringCount = readVarInt(&data);
    printf("String count: %d\n", stringCount);

    char **strings = malloc(stringCount * sizeof(char*));
    for (int i = 0; i < stringCount; i++)
    {
        int length = readVarInt(&data);
        strings[i] = malloc(length+1);
        strings[i][length] = 0;
        memcpy(strings[i], data, length);
        printf("String %d: %.*s\n", i, length, strings[i]);
        data += length;
    }

    if (typesversion == 3)
    {
        uint8_t index = *(uint8_t*)data;
        data++;
        printf("index: %d\n", index);

        if (index != 0)
        {
            printf("TODO index != 0\n");
            return;
        }
    }

    int protoCount = readVarInt(&data);
    printf("Proto count: %d\n", protoCount);

    for (int i = 0; i < protoCount; i++)
    {
        uint8_t maxstacksize = *(uint8_t*)data;
        data++;
        uint8_t numparams = *(uint8_t*)data;
        data++;
        uint8_t nups = *(uint8_t*)data;
        data++;
        uint8_t is_vararg = *(uint8_t*)data;
        data++;

        printf("Proto %d:\n", i);
        printf("Max stack size: %d\n", maxstacksize);
        printf("Numparams: %d\n", numparams);
        printf("Nups: %d\n", nups);
        printf("IsVararg? %d\n", is_vararg);

        if (version >= 4)
        {
            uint8_t flags = *(uint8_t*)data;
            data++;

            printf("Flags: %#x\n", flags);

            if (typesversion == 1)
            {
                printf("Invalid typesversion. (%d)\n", typesversion);
                return;
            }
            else if (typesversion == 2 || typesversion == 3)
            {
                uint32_t typesize = readVarInt(&data);

                printf("Type size: %d\n", typesize);

                if (typesize)
                {
                    if (typesversion == 3)
                        remapUserdataTypes(data, typesize, LBC_TYPE_TAGGED_USERDATA_END - LBC_TYPE_TAGGED_USERDATA_BASE);
                    data += typesize;
                }
            }
        }

        int sizecode = readVarInt(&data);
        printf("Sizecode: %d\n", sizecode);

        void *code = data;

        

        data += 4*sizecode;
    
        int sizek = readVarInt(&data);
        printf("Sizek: %d\n", sizek);

        LuauConstant *constants = malloc(sizek * sizeof(LuauConstant));
        if (sizek) constants->constantCount = sizek;

        for (int j = 0; j < sizek; j++)
        {
            uint8_t type = *(uint8_t*)data;
            data++;

            printf("Constant %d: ", j);

            constants[j].type = type;

            switch (type)
            {
                case LBC_CONSTANT_NIL:
                {
                    printf("nil\n");
                } break;
                case LBC_CONSTANT_STRING:
                {
                    int strId = readVarInt(&data) - 1;
                    printf("str %d ", strId);
                    printf("\"%s\"\n", strings[strId]);
                    constants[j].val = strings[strId];
                } break;
                case LBC_CONSTANT_IMPORT:
                {
                    uint32_t iid = *(uint32_t*)data;
                    data += sizeof(uint32_t);

                    printf("import count %d, id0 %d, id1 %d, id2 %d\n", (iid >> 30), (iid >> 20) & 1023, (iid >> 10) & 1023, iid & 1023);
                } break;
                case LBC_CONSTANT_TABLE:
                {
                    int keys = readVarInt(&data);
                    printf("tbl, len %d\n", keys);
                    for (int k = 0; k < keys; k++)
                    {
                        int key = readVarInt(&data);
                        printf("[%d] = [constant %d]\n", k, key);
                    }
                } break;
                case LBC_CONSTANT_CLOSURE:
                {
                    uint32_t fid = readVarInt(&data);
                    printf("closure %d\n", fid);
                } break;
                default:
                {
                    printf("Unknown constant type %d\n", type);
                    return;
                } break;
            }
        }

        int sizep = readVarInt(&data);
        printf("Sizep: %d\n", sizep);

        if (sizep)
        {
            for (int j = 0; j < sizep; j++)
            {
                readVarInt(&data);
            }
            printf("TODO Sizep\n");
        }

        disassemblecode(code, sizecode, constants);

        int linedefined = readVarInt(&data);
        printf("linedefined: %d\n", linedefined);

        char *debugname = readString(&data, strings);
        //printf("debugname: %s\n", debugname);

        uint8_t lineinfo = *(uint8_t*)data;
        data++;

        if (lineinfo)
        {
            uint8_t linegaplog2 = *(uint8_t*)data;
            data++;

            int intervals = ((sizecode - 1) >> linegaplog2) + 1;
            int absoffset = (sizecode + 3) & ~3;

            const int sizelineinfo = absoffset + intervals * sizeof(int);

            data += sizecode + intervals*4;

            printf("TODO lineinfo\n");
        }

        uint8_t debuginfo = *(uint8_t*)data;
        data++;

        if (debuginfo)
        {
            printf("TODO debuginfo\n");
        }
    }

    uint32_t mainid = readVarInt(&data);

    printf("Entrypoint proto: %d\n", mainid);
}

static void run_script(Script *script, const char *source, int sourceLength, bool client, bool plugin)
{
    lua_State *L = luaL_newstate();

    init_lua_state(L, script, client, plugin);

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
        printf("There was a problem: %d\n", result);
        goto end;
    }
    luaL_openlibs(L);
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
    }
    lua_close(L);
}

typedef struct ScriptRunnerArgs {
    Script *script;
    const char *source;
    int sourceLength;
    bool client;
    bool plugin;
    ScriptRuntime *this;
} ScriptRunnerArgs;

void *run_script_runner(void *args)
{
    ScriptRunnerArgs *srargs = args;

    run_script(srargs->script, srargs->source, srargs->sourceLength, srargs->client, srargs->plugin);

    Instance_Destroy(srargs->this);

    return NULL;
}

static void ScriptRuntime_RunScriptInternal(ScriptRuntime *this, Script *script, bool plugin)
{
    ScriptRunnerArgs *args = malloc(sizeof(ScriptRunnerArgs));
    pthread_t thread;

    args->script = script;
    args->source = script->Source;
    args->sourceLength = script->sourceLength;
    args->client = false;
    args->plugin = plugin;
    args->this = this;

    pthread_create(&thread, NULL, run_script_runner, args);
}

void ScriptRuntime_RunScript(ScriptRuntime *this, Script *script)
{
    ScriptRuntime_RunScriptInternal(this, script, false);
}

void ScriptRuntime_RunPluginScript(ScriptRuntime *this, Script *script)
{
    ScriptRuntime_RunScriptInternal(this, script, true);
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

