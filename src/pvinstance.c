#include "pvinstance.h"

PVInstance *PVInstance_new(const char *className, Instance *parent)
{
    PVInstance *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(PVInstance);

    return newInst;
}
