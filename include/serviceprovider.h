#ifndef _SERVICEPROVIDER_
#define _SERVICEPROVIDER_

#include "instance.h"

typedef struct ServiceProvider {
    Instance instance;

    RBXScriptSignal *Close;
    RBXScriptSignal *CloseLate;
    RBXScriptSignal *ServiceAdded;
    RBXScriptSignal *ServiceRemoving;
} ServiceProvider;

ServiceProvider *ServiceProvider_new(const char *className, Instance *parent);

Instance *ServiceProvider_GetService(ServiceProvider *this, const char *className);

#endif
