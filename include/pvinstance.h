#ifndef _PVINSTANCE_
#define _PVINSTANCE_

#include "instance.h"
#include "cframe.h"

typedef struct PVInstance {
    Instance instance;

    CoordinateFrame CoordinateFrame;
} PVInstance;

PVInstance *PVInstance_new(const char *className, Instance *parent);

#endif
