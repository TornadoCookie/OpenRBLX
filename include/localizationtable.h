#ifndef _LOCALIZATIONTABLE_
#define _LOCALIZATIONTABLE_

#include "instance.h"

typedef struct LocalizationTable {
    Instance instance;
} LocalizationTable;

LocalizationTable *LocalizationTable_new(const char *className, Instance *parent);
void serialize_LocalizationTable(LocalizationTable *localizationtable, SerializeInstance *inst);

#endif

