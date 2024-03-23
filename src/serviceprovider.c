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
    for (int i = 0; i < this->instance.childCount; i++)
    {
        if (!strcmp(this->instance.children[i]->ClassName, className)) return this->instance.children[i];
    }

    return NULL;
}
