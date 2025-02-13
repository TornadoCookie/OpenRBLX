#ifndef _STARTERPLAYER_
#define _STARTERPLAYER_

#include "instance.h"

typedef struct StarterPlayer {
    Instance instance;
} StarterPlayer;

StarterPlayer *StarterPlayer_new(const char *className, Instance *parent);

void serialize_StarterPlayer(StarterPlayer *starterplayer, SerializeInstance *inst);

#endif

