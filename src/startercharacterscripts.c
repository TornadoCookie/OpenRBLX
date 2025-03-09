#include "startercharacterscripts.h"

StarterCharacterScripts *StarterCharacterScripts_new(const char *className, Instance *parent)
{
    StarterCharacterScripts *newInst = Instance_new(className, parent);

    newInst->instance.Name = "StarterCharacterScripts";
    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

void serialize_StarterCharacterScripts(StarterCharacterScripts *startercharacterscripts, SerializeInstance *inst)
{
    serialize_Instance(startercharacterscripts, inst);
}


