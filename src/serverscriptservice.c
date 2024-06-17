#include "serverscriptservice.h"
#include "datamodel.h"
#include <string.h>
#include "script.h"

#define LUA_IMPL
#include "minilua.h"

ServerScriptService *ServerScriptService_new(const char *className, Instance *parent)
{
    ServerScriptService *newInst = Instance_new("ServerScriptService", parent);

    newInst->instance.DataCost = sizeof(ServerScriptService);
    newInst = realloc(newInst, sizeof(ServerScriptService));

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

static void run_script(Script *script)
{
    lua_State *L = luaL_newstate();

    luaL_openlibs(L);

    luaL_loadbuffer(L, script->Source, script->sourceLength, Instance_GetFullName(script));
    lua_pcall(L, 0, 0, 0);

}

static void run_scripts(Instance *inst)
{
    if (!strcmp(inst->ClassName, "Script")) run_script(inst);

    for (int i = 0; i < inst->childCount; i++)
    {
        run_scripts(inst->children[i]);
    }
}

void ServerScriptService_RunScripts(ServerScriptService *this)
{
    run_scripts(this);
}
