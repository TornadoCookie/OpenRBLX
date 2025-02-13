#include "starterplayerscripts.h"

StarterPlayerScripts *StarterPlayerScripts_new(const char *className, Instance *parent)
{
    StarterPlayerScripts *newInst = Instance_new(className, parent);
    if (parent) Instance_SetParent(newInst, parent);

    // No need to change DataCost or size as no members

    return newInst;
}

void serialize_StarterPlayerScripts(StarterPlayerScripts *starterplayerscripts, SerializeInstance *inst)
{
    serialize_Instance(starterplayerscripts, inst);
}

