#include "physicalcharacter.h"
#include <stdlib.h>

PhysicalCharacter *PhysicalCharacter_new(Instance *parent)
{
    PhysicalCharacter *newInst = Model_new("PhysicalCharacter", parent);

    newInst->model.pvinstance.instance.DataCost = sizeof(PhysicalCharacter);
    newInst = realloc(newInst, sizeof(PhysicalCharacter));

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}
