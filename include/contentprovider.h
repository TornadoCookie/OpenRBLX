#ifndef _CONTENTPROVIDER_
#define _CONTENTPROVIDER_

#include "instance.h"

typedef struct ContentProvider {
    Instance instance;

    const char *BaseUrl;
} ContentProvider;

ContentProvider *ContentProvider_new(const char *className, Instance *parent);
void serialize_ContentProvider(ContentProvider *contentprovider, SerializeInstance *inst);

#endif

