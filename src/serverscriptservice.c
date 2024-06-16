#include "serverscriptservice.h"
#include "datamodel.h"
#include <string.h>

ServerScriptService *ServerScriptService_new(const char *className, Instance *parent)
{
    ServerScriptService *newInst = Instance_new("ServerScriptService", parent);

    newInst->instance.DataCost = sizeof(ServerScriptService);
    newInst = realloc(newInst, sizeof(ServerScriptService));

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

static void run_script(void *script)
{

}

void ServerScriptService_RunScripts(ServerScriptService *this)
{
    int childCount;
    Instance **descendants = Instance_GetDescendants(GetDataModel(), &childCount);

    for (int i = 0; i < childCount; i++)
    {
        if (!strcmp(descendants[i]->ClassName, "Script"))
        {
            run_script(descendants[i]);
        }
    }
}
