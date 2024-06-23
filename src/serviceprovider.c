#include "serviceprovider.h"
#include <string.h>
#include <stdlib.h>

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
    Instance *ret = Instance_FindFirstChildOfClass(this, className);

    if (!ret)
    {
        ret = Instance_dynNew(className, this);
    }

    return ret;
}
