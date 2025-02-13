#include "starterplayer.h"

StarterPlayer *StarterPlayer_new(const char *className, Instance *parent)
{
    StarterPlayer *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

void serialize_StarterPlayer(StarterPlayer *starterplayer, SerializeInstance *inst)
{
    serialize_Instance(starterplayer, inst);
}

