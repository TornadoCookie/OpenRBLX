#ifndef _STANDALONEPLUGINSCRIPTS_
#define _STANDALONEPLUGINSCRIPTS_

#include "instance.h"

typedef struct StandalonePluginScripts {
    Instance instance;
} StandalonePluginScripts;

StandalonePluginScripts *StandalonePluginScripts_new(const char *className, Instance *parent);

#endif

