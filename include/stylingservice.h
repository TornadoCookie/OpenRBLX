#ifndef _STYLINGSERVICE_
#define _STYLINGSERVICE_

#include "instance.h"

typedef struct StylingService {
    Instance instance;
} StylingService;

StylingService *StylingService_new(const char *className, Instance *parent);

#endif

