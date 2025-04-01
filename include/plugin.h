#ifndef _PLUGIN_
#define _PLUGIN_

#include "instance.h"

typedef struct Plugin {
    Instance instance;
} Plugin;

Plugin *Plugin_new(const char *className, Instance *parent);
void serialize_Plugin(Plugin *plugin, SerializeInstance *inst);

#endif

