#include "localizationservice.h"

LocalizationService *LocalizationService_new(const char *className, Instance *parent)
{
    LocalizationService *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    if (parent) Instance_SetParent(newInst, parent);

    newInst->RobloxLocaleId = "en-us";

    return newInst;
}

void serialize_LocalizationService(LocalizationService *localizationservice, SerializeInstance *inst)
{
    serialize_Instance(localizationservice, inst);

    serialize_atomic(string, localizationservice, RobloxLocaleId);
}

