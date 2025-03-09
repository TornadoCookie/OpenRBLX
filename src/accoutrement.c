#include "accoutrement.h"

Accoutrement *Accoutrement_new(const char *className, Instance *parent)
{
    Accoutrement *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(Accoutrement);
    newInst = realloc(newInst, sizeof(*newInst));

    return newInst;
}

void serialize_Accoutrement(Accoutrement *accoutrement, SerializeInstance *inst)
{
    serialize_Instance(accoutrement, inst);

    serialize_atomic(CoordinateFrame, accoutrement, AttachmentPoint);
}



