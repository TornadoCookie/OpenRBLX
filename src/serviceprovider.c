#include "serviceprovider.h"
#include <string.h>
#include <stdlib.h>

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(serviceprovider);

ServiceProvider *ServiceProvider_new(const char *className, Instance *parent)
{
    ServiceProvider *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(ServiceProvider);
    newInst = realloc(newInst, sizeof(ServiceProvider));

    newInst->ServiceAdded = newInst->instance.ChildAdded;
    newInst->ServiceRemoving = newInst->instance.ChildRemoved;
    newInst->Close = RBXScriptSignal_new();
    newInst->CloseLate = RBXScriptSignal_new();

    return newInst;
}

Instance *ServiceProvider_GetService(ServiceProvider *this, const char *className)
{
    // HttpRbxApiService is HttpService, but it can access roblox.com
    // We do not care
    if (!strcmp(className, "HttpRbxApiService"))
    {
        className = "HttpService";
    }

    Instance *ret = Instance_FindFirstChildOfClass(this, className);

    if (!ret)
    {
        FIXME("creating %s\n", className);
        ret = Instance_dynNew(className, this);
    }

    return ret;
}
