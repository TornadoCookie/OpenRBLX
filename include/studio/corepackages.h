#ifndef _COREPACKAGES_
#define _COREPACKAGES_

#include "instance.h"

typedef struct CorePackages {
    Instance instance;
} CorePackages;

CorePackages *CorePackages_new(const char *className, Instance *parent);

#endif

