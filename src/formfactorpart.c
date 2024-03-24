#include "formfactorpart.h"
#include <stdlib.h>

FormFactorPart *FormFactorPart_new(const char *className, Instance *parent)
{
    FormFactorPart *newInst = BasePart_new(className, parent);

    newInst->basepart.pvinstance.instance.DataCost = sizeof(FormFactorPart);
    newInst = realloc(newInst, sizeof(FormFactorPart));

    return newInst;
}
