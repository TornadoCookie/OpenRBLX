#include "pluginmouse.h"

PluginMouse *PluginMouse_new(const char *className, Instance *parent)
{
    INSTANCE_STUB_CONSTRUCTOR(PluginMouse, Mouse);
}

void serialize_PluginMouse(PluginMouse *pluginmouse, SerializeInstance *inst)
{
    serialize_Mouse(pluginmouse, inst);
}

