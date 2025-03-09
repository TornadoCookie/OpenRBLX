#ifndef _ACCOUTREMENT_
#define _ACCOUTREMENT_

#include "instance.h"

typedef struct Accoutrement {
    Instance instance;

    CFrame AttachmentPoint;
} Accoutrement;

Accoutrement *Accoutrement_new(const char *className, Instance *parent);
void serialize_Accoutrement(Accoutrement *accoutrement, SerializeInstance *inst);

#endif

