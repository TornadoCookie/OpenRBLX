#ifndef _PVINSTANCE_
#define _PVINSTANCE_

#include "instance.h"

typedef struct PVInstance {
    Instance instance;
} PVInstance;

PVInstance *PVInstance_new(const char *className, Instance *parent);

#endif
