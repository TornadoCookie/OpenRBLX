#ifndef _MESSAGEBUSSERVICE_
#define _MESSAGEBUSSERVICE_

#include "instance.h"

typedef struct MessageBusService {
    Instance instance;
} MessageBusService;

MessageBusService *MessageBusService_new(const char *className, Instance *parent);

#endif

