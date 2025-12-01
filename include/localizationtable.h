#ifndef _LOCALIZATIONTABLE_
#define _LOCALIZATIONTABLE_

#include "instance.h"
#include "translator.h"

typedef struct LocalizationTable {
    Instance instance;
} LocalizationTable;

LocalizationTable *LocalizationTable_new(const char *className, Instance *parent);

Translator *LocalizationTable_GetTranslator(LocalizationTable *this, const char *localeId);

void serialize_LocalizationTable(LocalizationTable *localizationtable, SerializeInstance *inst);

#endif

