#include "messagebusservice.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(messagebusservice);

MessageBusService *MessageBusService_new(const char *className, Instance *parent)
{
    INSTANCE_STUB_CONSTRUCTOR(MessageBusService, Instance);
}

const char *MessageBusService_GetMessageId(MessageBusService *this, const char *domainName, const char *messageName)
{
    FIXME("domainName %s, messageName %s stub!\n", domainName, messageName);
    return "huh?";
}

