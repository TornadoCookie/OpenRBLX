#ifndef _MESSAGEBUSSERVICE_
#define _MESSAGEBUSSERVICE_

#include "instance.h"

typedef struct MessageBusService {
    Instance instance;
} MessageBusService;

MessageBusService *MessageBusService_new(const char *className, Instance *parent);

const char *MessageBusService_GetMessageId(MessageBusService *this, const char *domainName, const char *messageName);

#endif

