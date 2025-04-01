#include "localizationtable.h"

LocalizationTable *LocalizationTable_new(const char *className, Instance *parent)
{
    LocalizationTable *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    if (parent)
        Instance_SetParent(newInst, parent);

    return newInst;
}

void serialize_LocalizationTable(LocalizationTable *localizationtable, SerializeInstance *inst)
{
    serialize_Instance(localizationtable, inst);
}

