#include "serverscriptservice.h"
#include "datamodel.h"
#include <string.h>
#include "script.h"
#include <pthread.h>
#include <time.h>
#include "scriptruntime.h"

ServerScriptService *ServerScriptService_new(const char *className, Instance *parent)
{
    ServerScriptService *newInst = Instance_new("ServerScriptService", parent);

    newInst->instance.DataCost = sizeof(ServerScriptService);
    newInst = realloc(newInst, sizeof(ServerScriptService));

   // time_initialized = time(NULL);

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

static void run_scripts(Instance *inst, ServerScriptService *this)
{
    if (!inst) return;

    for (int i = 0; i < inst->childCount; i++)
    {
        run_scripts(inst->children[i], this);
    }

    if (!strcmp(inst->ClassName, "Script"))
    {
        ScriptRuntime *scrRuntime = ScriptRuntime_new("ScriptRuntime", this);
        ScriptRuntime_RunScript(scrRuntime, inst);
    }
}

void ServerScriptService_RunScripts(ServerScriptService *this)
{
    run_scripts(GetDataModel(), this);
}

