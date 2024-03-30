#include "sky.h"
#include <stdlib.h>

Sky *Sky_new(Instance *parent)
{
    Sky *newInst = Instance_new("Sky", parent);

    newInst->instance.DataCost = sizeof(Sky);
    newInst = realloc(newInst, sizeof(Sky));

    return newInst;
}
