#include "rootinstance.h"

RootInstance *RootInstance_new(const char *className, Instance *parent)
{
    RootInstance *newInst = Model_new(className, parent);

    newInst->model.pvinstance.instance.DataCost = sizeof(RootInstance);

    return newInst;
}

void serialize_RootInstance(RootInstance *rootinstance, SerializeInstance *inst)
{
    serialize_Model(rootinstance, inst);
}
