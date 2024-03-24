#ifndef _PVINSTANCE_
#define _PVINSTANCE_

#include "instance.h"
#include "cframe.h"

typedef struct PVInstance {
    Instance instance;

    int Controller;
    bool ControllerFlagShown;
} PVInstance;

PVInstance *PVInstance_new(const char *className, Instance *parent);

#endif
