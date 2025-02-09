#include "spawnlocation.h"

#include "debug.h"

DEFAULT_DEBUG_CHANNEL(spawnlocation);

SpawnLocation *SpawnLocation_new(const char *className, Instance *parent)
{
    SpawnLocation *newInst = Part_new(className, parent);

    newInst->part.formfactorpart.basepart.pvinstance.instance.DataCost = sizeof(SpawnLocation);
    newInst = realloc(newInst, sizeof(*newInst));

    newInst->AllowTeamChangeOnTouch = false;
    newInst->Duration = 10;
    newInst->Enabled = true;
    newInst->Neutral = true;

    return newInst;
}

void serialize_SpawnLocation(SpawnLocation *spawnlocation, SerializeInstance *inst)
{
    serialize_Part(spawnlocation, inst);
}

