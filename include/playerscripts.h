#ifndef _PLAYERSCRIPTS_
#define _PLAYERSCRIPTS_

#include "instance.h"
typedef struct PlayerScripts {
    Instance instance;
} PlayerScripts;

PlayerScripts *PlayerScripts_new(const char *className, Instance *parent);

// no serialize, this is a runtime class

#endif

