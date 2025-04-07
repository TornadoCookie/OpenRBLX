#ifndef _STARTPAGESERVICE_
#define _STARTPAGESERVICE_

#include "instance.h"

typedef struct StartPageService {
    Instance instance;
} StartPageService;

StartPageService *StartPageService_new(const char *className, Instance *parent);

#endif

