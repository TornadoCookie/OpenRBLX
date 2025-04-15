#include "plugin.h"

Plugin *Plugin_new(const char *className, Instance *parent)
{
    Plugin *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    newInst->mouse = PluginMouse_new("PluginMouse", newInst);

    if (parent)
        Instance_SetParent(newInst, parent);
    
    return newInst;
}

PluginMouse *Plugin_GetMouse(Plugin *this)
{
    return this->mouse;
}

void serialize_Plugin(Plugin *plugin, SerializeInstance *inst)
{
    serialize_Instance(plugin, inst);
}

