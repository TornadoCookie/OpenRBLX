#ifndef _GLOBALSETTINGS_
#define _GLOBALSETTINGS_

#include "genericsettings.h"

typedef struct GlobalSettings {
    GenericSettings genericsettings;
} GlobalSettings;

GlobalSettings *GetGlobalSettings(void);

const char *GlobalSettings_GetFVariable(GlobalSettings *this, const char *name);

#endif

