#include "trusspart.h"

TrussPart *TrussPart_new(Instance *parent)
{
    TrussPart *newInst = BasePart_new("TrussPart", parent);

    newInst->basepart.pvinstance.instance.DataCost = sizeof(TrussPart);

    return newInst;
}
