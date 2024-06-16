#include "valuebase.h"

ValueBase *ValueBase_new(const char *className, Instance *parent)
{
    ValueBase *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(ValueBase);
    newInst = realloc(newInst, sizeof(ValueBase));

    return newInst;
}

void serialize_ValueBase(ValueBase *valuebase, SerializeInstance *inst)
{
    serialize_Instance(valuebase, inst);
}
