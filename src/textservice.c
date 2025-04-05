#include "textservice.h"

TextService *TextService_new(const char *className, Instance *parent)
{
    TextService *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    if (parent)
        Instance_SetParent(newInst, parent);

    return newInst;
}

