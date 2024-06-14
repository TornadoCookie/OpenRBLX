#include "formfactorpart.h"
#include <stdlib.h>
#include "serialize.h"

FormFactorPart *FormFactorPart_new(const char *className, Instance *parent)
{
    FormFactorPart *newInst = BasePart_new(className, parent);

    newInst->basepart.pvinstance.instance.DataCost = sizeof(FormFactorPart);
    newInst = realloc(newInst, sizeof(FormFactorPart));

    return newInst;
}

void serialize_FormFactorPart(FormFactorPart *formfactorpart, SerializeInstance *inst)
{
    serialize_BasePart(formfactorpart, inst);
}
