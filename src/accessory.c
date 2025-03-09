#include "accessory.h"

Accessory *Accessory_new(const char *className, Instance *parent)
{
    Accessory *newInst = Accoutrement_new(className, parent);

    newInst->accoutrement.instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    return newInst;
}

void serialize_Accessory(Accessory *accessory, SerializeInstance *inst)
{
    serialize_Accoutrement(accessory, inst);

    serialize_atomic(token, accessory, AccessoryType);

}

