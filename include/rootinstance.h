#ifndef _ROOTINSTANCE_
#define _ROOTINSTANCE_

#include "model.h"

typedef struct RootInstance {
    Model_Instance model;
} RootInstance;

RootInstance *RootInstance_new(const char *className, Instance *parent);

#endif
