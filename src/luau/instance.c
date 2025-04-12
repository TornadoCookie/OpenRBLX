#include "luau/serialize.h"
#include "luau/instance.h"
#include "luau/event.h"

#include "plugin.h"
#include "globalsettings.h"
#include "datamodel.h"
#include "messagebusservice.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(luau);

#include "lualib.h"

Instance *luau_toinstance(lua_State *L, int i)
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
    else if (!strcmp(name, "Source"))
    {
        lua_pushstring(L, ""); // TODO this may be just in the case of the script being bytecode
                               // DocParser otherwise crashes
        return 1;
    }

    SerializeInstance sInst = { 0 };
    Instance_Serialize(inst, &sInst);

    for (int i = 0; i < sInst.serializationCount; i++)
    {
        if (!strcmp(name, sInst.serializations[i].name))
        {
             return luau_pushserialization(L, sInst.serializations[i]);
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

    SerializeInstance sInst = { 0 };
    Instance_Serialize(inst, &sInst);

    for (int i = 0; i < sInst.serializationCount; i++)
    {
        if (!strcmp(key, sInst.serializations[i].name))
        {
            Serialization sz = luau_toserialization(L, 3);
            int ds = sztypesiz(sz.type);

            memcpy(sInst.serializations[i].val, sz.val, ds);
        }
    }

    free(sInst.serializations);

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
        lua_pushinteger(L, i);
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

static int luau_Instance_IsA(lua_State *L)
{
    int nargs = lua_gettop(L);
    if (nargs != 2)
    {
        lua_pushstring(L, "Expected 2 arguments.\n");
        lua_error(L);
    }

    Instance *inst = luau_toinstance(L, 1);
    const char *className = lua_tostring(L, 2);

    lua_pushboolean(L, Instance_IsA(inst, className));

    return 1;
}


static int luau_Instance_SetAttribute(lua_State *L)
{
    Instance *inst = luau_toinstance(L, 1);
    const char *attributeName = lua_tostring(L, 2);

    Serialization sz = luau_toserialization(L, 3);
    sz.name = attributeName;

    FIXME("attr %s, type %d\n", sz.name, sz.type);

    Instance_SetAttribute(inst, sz);

    return 0;
}

static int luau_Instance_GetAttribute(lua_State *L)
{
    Instance *inst = luau_toinstance(L, 1);
    const char *attributeName = lua_tostring(L, 2); 

    return luau_pushserialization(L, Instance_GetAttribute(inst, attributeName));
}

static int luau_Instance_GetPropertyChangedSignal(lua_State *L)
{
    Instance *inst = luau_toinstance(L, 1);
    const char *prop = lua_tostring(L, 2);

    luau_pushevent(L, Instance_GetPropertyChangedSignal(inst, prop));

    return 1;
}

static int luau_ServiceProvider_GetService(lua_State *L)
{
    ServiceProvider *serviceProvider = luau_toinstance(L, 1); 
    const char *serviceName = lua_tostring(L, 2);

    Instance *service = ServiceProvider_GetService(serviceProvider, strdup(serviceName));

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

static int luau_DataModel_GetFastString(lua_State *L)
{
    DataModel *game = luau_toinstance(L, 1);
    const char *name = lua_tostring(L, 2);

    lua_pushstring(L, DataModel_GetFastString(game, name));

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

static int luau_DataModel_DefineFastString(lua_State *L)
{
    DataModel *game = luau_toinstance(L, 1);
    const char *name = lua_tostring(L, 2);
    const char *defaultValue = lua_tostring(L, 3);

    lua_pushstring(L, DataModel_DefineFastString(game, name, defaultValue));

    return 1;
}

static int luau_DataModel_GetEngineFeature(lua_State *L)
{
    DataModel *game = luau_toinstance(L, 1);
    const char *name = lua_tostring(L, 2);

    lua_pushboolean(L, DataModel_GetEngineFeature(game, name));
    
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

static int luau_GlobalSettings_GetFVariable(lua_State *L)
{
    GlobalSettings *settings = luau_toinstance(L, 1);
    const char *name = lua_tostring(L, 1);

    lua_pushstring(L, GlobalSettings_GetFVariable(settings, name));

    return 1;
}

static int luau_StyleRule_SetProperties(lua_State *L)
{
    FIXME("state %p\n", L);
    return 0;
}

static int luau_StudioService_GetClassIcon(lua_State *L)
{
    FIXME("className %s\n", lua_tostring(L, 2));
    return 0;
}

static int luau_MessageBusService_GetMessageId(lua_State *L)
{
    MessageBusService *mbs = luau_toinstance(L, 1);
    const char *domainName = lua_tostring(L, 2);
    const char *messageName = lua_tostring(L, 3);

    lua_pushstring(L, MessageBusService_GetMessageId(mbs, domainName, messageName));

    return 1;
}

static int luau_HttpService_RequestInternal(lua_State *L)
{
    FIXME("state %p\n", L);
    return 0;
}

static int luau_LocalizationTable_GetTranslator(lua_State *L)
{
    FIXME("state %p\n", L);
    return 0;
}

void luau_pushinstance(lua_State *L, Instance *inst)
{
    if (!inst)
    {
        lua_pushnil(L);
        return;
    }

    lua_newtable(L);

    lua_pushcfunction(L, luau_Instance_FindFirstChild, "Instance:FindFirstChild");
    lua_setfield(L, -2, "FindFirstChild");

    lua_pushcfunction(L, luau_Instance_FindFirstChild, "Instance:findFirstChild");
    lua_setfield(L, -2, "findFirstChild");

    lua_pushcfunction(L, luau_Instance_FindFirstAncestor, "Instance:FindFirstAncestor");
    lua_setfield(L, -2, "FindFirstAncestor");

    lua_pushcfunction(L, luau_Instance_FindFirstAncestorWhichIsA, "Instance:FindFirstAncestorWhichIsA");
    lua_setfield(L, -2, "FindFirstAncestorWhichIsA");

    lua_pushcfunction(L, luau_Instance_FindFirstChildWhichIsA, "Instance:FindFirstChildWhichIsA");
    lua_setfield(L, -2, "FindFirstChildWhichIsA");

    lua_pushcfunction(L, luau_Instance_GetChildren, "Instance:GetChildren");
    lua_setfield(L, -2, "GetChildren");

    lua_pushcfunction(L, luau_Instance_GetChildren, "Instance:children");
    lua_setfield(L, -2, "children");

    lua_pushcfunction(L, luau_Instance_Clone, "Instance:Clone");
    lua_setfield(L, -2, "Clone");

    lua_pushcfunction(L, luau_Instance_WaitForChild, "Instance:WaitForChild");
    lua_setfield(L, -2, "WaitForChild");

    lua_pushcfunction(L, luau_Instance_IsDescendantOf, "Instance:IsDescendantOf");
    lua_setfield(L, -2, "IsDescendantOf");

    lua_pushcfunction(L, luau_Instance_IsA, "Instance:IsA");
    lua_setfield(L, -2, "IsA");

    lua_pushcfunction(L, luau_Instance_SetAttribute, "Instance:SetAttribute");
    lua_setfield(L, -2, "SetAttribute");

    lua_pushcfunction(L, luau_Instance_GetAttribute, "Instance:GetAttribute");
    lua_setfield(L, -2, "GetAttribute");

    lua_pushcfunction(L, luau_Instance_GetPropertyChangedSignal, "Instance:GetPropertyChangedSignal");
    lua_setfield(L, -2, "GetPropertyChangedSignal");

    lua_pushlightuserdata(L, inst);
    lua_setfield(L, -2, "__inst_ptr");

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
            
            lua_pushcfunction(L, luau_DataModel_GetFastString, "DataModel:GetFastString");
            lua_setfield(L, -2, "GetFastString");

            lua_pushcfunction(L, luau_DataModel_DefineFastString, "DataModel:DefineFastString");
            lua_setfield(L, -2, "DefineFastString");

            lua_pushcfunction(L, luau_DataModel_GetEngineFeature, "DataModel:GetEngineFeature");
            lua_setfield(L, -2, "GetEngineFeature");
        }

        if (!strcmp(inst->ClassName, "GlobalSettings"))
        {
            lua_pushcfunction(L, luau_GlobalSettings_GetFVariable, "GlobalSettings:GetFVariable");
            lua_setfield(L, -2, "GetFVariable");
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

    if (!strcmp(inst->ClassName, "StyleRule"))
    {
        lua_pushcfunction(L, luau_StyleRule_SetProperties, "StyleRule:SetProperties");
        lua_setfield(L, -2, "SetProperties");
    }

    if (!strcmp(inst->ClassName, "StudioService"))
    {
        lua_pushcfunction(L, luau_StudioService_GetClassIcon, "StudioService:GetClassIcon");
        lua_setfield(L, -2, "GetClassIcon");
    }

    if (!strcmp(inst->ClassName, "MessageBusService"))
    {
        lua_pushcfunction(L, luau_MessageBusService_GetMessageId, "MessageBusService:GetMessageId");
        lua_setfield(L, -2, "GetMessageId");
    }

    if (!strcmp(inst->ClassName, "HttpService"))
    {
        lua_pushcfunction(L, luau_HttpService_RequestInternal, "HttpService:RequestInternal");
        lua_setfield(L, -2, "RequestInternal");
    }

    if (!strcmp(inst->ClassName, "LocalizationTable"))
    {
        lua_pushcfunction(L, luau_LocalizationTable_GetTranslator, "LocalizationTable:GetTranslator");
        lua_setfield(L, -2, "GetTranslator");
    }

    lua_newtable(L);

    lua_pushcfunction(L, luau_Instance__index, "Instance:__index");
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, luau_Instance__newindex, "Instance:__newindex");
    lua_setfield(L, -2, "__newindex");

    lua_setmetatable(L, -2);
}

int luau_Instance_new(lua_State *L)
{
    const char *className = lua_tostring(L, 1);
    Instance *parent = NULL;

    if (lua_istable(L, 2))
    {
        parent = luau_toinstance(L, 2);
    }

    Instance *newInst = Instance_dynNew(className, parent);

    if (!newInst)
    {
        luaL_error(L, "Unable to create instance. (class not implemented?)\n");
    }

    luau_pushinstance(L, newInst);
    return 1;
}

