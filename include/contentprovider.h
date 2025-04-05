#ifndef _CONTENTPROVIDER_
#define _CONTENTPROVIDER_

#include "instance.h"

typedef struct ContentProvider {
    Instance instance;
} ContentProvider;

ContentProvider *ContentProvider_new(const char *className, Instance *parent);

#endif

