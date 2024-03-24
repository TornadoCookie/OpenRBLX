#include "part.h"
#include <stdlib.h>

Part *Part_new(Instance *parent)
{
    Part *newInst = FormFactorPart_new("Part", parent);

    newInst->formfactorpart.basepart.pvinstance.instance.DataCost = sizeof(Part);
    newInst = realloc(newInst, sizeof(Part));

    return newInst;
}
