#ifndef _PVINSTANCE_
#define _PVINSTANCE_

#include "instance.h"
#include "cframe.h"

typedef struct PVInstance PVInstance;

typedef struct PVInstance {
    Instance instance;

    int Controller;
    bool ControllerFlagShown;
    void (*drawFunc)(PVInstance *);
} PVInstance;

PVInstance *PVInstance_new(const char *className, Instance *parent);
void PVInstance_Draw(PVInstance *this);

#endif
