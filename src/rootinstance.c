#include "rootinstance.h"

RootInstance *RootInstance_new(const char *className, Instance *parent)
{
    RootInstance *newInst = Model_new(className, parent);

    newInst->model.pvinstance.instance.DataCost = sizeof(RootInstance);

    return newInst;
}
