#include "plugin.h"

Plugin *Plugin_new(const char *className, Instance *parent)
{
    Plugin *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    if (parent)
        Instance_SetParent(newInst, parent);
    
    return newInst;
}

void serialize_Plugin(Plugin *plugin, SerializeInstance *inst)
{
    serialize_Instance(plugin, inst);
}

