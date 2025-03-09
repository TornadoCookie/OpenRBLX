#ifndef _STARTERCHARACTERSCRIPTS_
#define _STARTERCHARACTERSCRIPTS_

#include "instance.h"

typedef struct StarterCharacterScripts {
    Instance instance;
} StarterCharacterScripts;

StarterCharacterScripts *StarterCharacterScripts_new(const char *className, Instance *parent);
void serialize_StarterCharacterScripts(StarterCharacterScripts *startercharacterscripts, SerializeInstance *inst);

#endif

