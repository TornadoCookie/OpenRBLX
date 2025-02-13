#ifndef _STARTERPLAYERSCRIPTS_
#define _STARTERPLAYERSCRIPTS_

#include "instance.h"

typedef struct StarterPlayerScripts {
    Instance instance;
} StarterPlayerScripts;

StarterPlayerScripts *StarterPlayerScripts_new(const char *className, Instance *parent);

void serialize_StarterPlayerScripts(StarterPlayerScripts *starterplayerscripts, SerializeInstance *inst);

#endif

