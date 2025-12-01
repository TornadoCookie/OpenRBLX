#include "coregui.h"

CoreGui *CoreGui_new(const char *className, Instance *parent)
{
    CoreGui *newInst = BasePlayerGui_new(className, parent);

    newInst->baseplayergui.instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    newInst->baseplayergui.instance.Name = "CoreGui";

    if (parent)
        Instance_SetParent(newInst, parent);

    return newInst;
}

void serialize_CoreGui(CoreGui *coregui, SerializeInstance *inst)
{
    serialize_BasePlayerGui(coregui, inst);
}

