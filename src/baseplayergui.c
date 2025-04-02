#include "baseplayergui.h"

BasePlayerGui *BasePlayerGui_new(const char *className, Instance *parent)
{
    BasePlayerGui *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    return newInst;
}

