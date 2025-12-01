#include "localizationtable.h"
#include "debug.h"

DEFAULT_DEBUG_CHANNEL(localizationtable);

LocalizationTable *LocalizationTable_new(const char *className, Instance *parent)
{
    LocalizationTable *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    if (parent)
        Instance_SetParent(newInst, parent);

    return newInst;
}

Translator *LocalizationTable_GetTranslator(LocalizationTable *this, const char *localeId)
{
    FIXME("localeId %s\n", localeId);

    return Translator_new("Translator", NULL);
}

void serialize_LocalizationTable(LocalizationTable *localizationtable, SerializeInstance *inst)
{
    serialize_Instance(localizationtable, inst);
}

