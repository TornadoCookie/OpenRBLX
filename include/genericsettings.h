#ifndef _GENERICSETTINGS_
#define _GENERICSETTINGS_

#include "serviceprovider.h"

typedef struct GenericSettings {
    ServiceProvider serviceprovider;
} GenericSettings;

GenericSettings *GenericSettings_new(const char *className, Instance *parent);

#endif

