#ifndef _STUDIOWIDGETSSERVICE_
#define _STUDIOWIDGETSSERVICE_

#include "instance.h"

typedef struct StudioWidgetsService {
    Instance instance;
} StudioWidgetsService;

StudioWidgetsService *StudioWidgetsService_new(const char *className, Instance *parent);

#endif

