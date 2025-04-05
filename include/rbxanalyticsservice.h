#ifndef _RBXANALYTICSSERVICE_
#define _RBXANALYTICSSERVICE_

#include "instance.h"

typedef struct RbxAnalyticsService {
    Instance instance;
} RbxAnalyticsService;

RbxAnalyticsService *RbxAnalyticsService_new(const char *className, Instance *parent);

#endif

