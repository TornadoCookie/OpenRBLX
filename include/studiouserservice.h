#ifndef _STUDIOUSERSERVICE_
#define _STUDIOUSERSERVICE_

#include "instance.h"

typedef struct StudioUserService {
    Instance instance;
} StudioUserService;

StudioUserService *StudioUserService_new(const char *className, Instance *parent);

#endif

