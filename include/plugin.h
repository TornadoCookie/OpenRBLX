#ifndef _PLUGIN_
#define _PLUGIN_

#include "instance.h"
#include "pluginmouse.h"

typedef struct Plugin {
    Instance instance;

    PluginMouse *mouse;
} Plugin;

Plugin *Plugin_new(const char *className, Instance *parent);

PluginMouse *Plugin_GetMouse(Plugin *this);

void serialize_Plugin(Plugin *plugin, SerializeInstance *inst);

#endif

