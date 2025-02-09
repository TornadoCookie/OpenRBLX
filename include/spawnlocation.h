#ifndef _SPAWNLOCATION_
#define _SPAWNLOCATION_

#include "part.h"
#include "brickcolor.h"

typedef struct SpawnLocation {
    Part part;

    bool AllowTeamChangeOnTouch;
    int Duration;
    bool Enabled;
    bool Neutral;
    BrickColor TeamColor;
} SpawnLocation;

SpawnLocation *SpawnLocation_new(const char *className, Instance *parent);

void serialize_SpawnLocation(SpawnLocation *spawnLocation, SerializeInstance *inst);

#endif

