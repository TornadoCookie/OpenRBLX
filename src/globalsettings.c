#include "globalsettings.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(globalsettings);

static GlobalSettings *settings;

GlobalSettings *GetGlobalSettings(void)
{
    if (!settings)
    {
        settings = GenericSettings_new("GlobalSettings", NULL);
    }

    return settings;
}

const char *GlobalSettings_GetFVariable(GlobalSettings *this, const char *name)
{
    FIXME("name %s\n", name);
    return "";
}

