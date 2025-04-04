#include "studio/corepackages.h"
#include "scriptruntime.h"

CorePackages *CorePackages_new(const char *className, Instance *parent)
{
    CorePackages *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    

    if (parent)
        Instance_SetParent(newInst, parent);

    return newInst;
}

