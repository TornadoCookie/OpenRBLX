#ifndef _HUMANOID_
#define _HUMANOID_

#include "instance.h"

typedef struct Humanoid {
    Instance instance;

    bool didBakeText;
    Texture2D bakedText;
} Humanoid;

Humanoid *Humanoid_new(const char *className, Instance *parent);
void serialize_Humanoid(Humanoid *humanoid, SerializeInstance *inst);

void Humanoid_Draw(Humanoid *humanoid);

#endif

