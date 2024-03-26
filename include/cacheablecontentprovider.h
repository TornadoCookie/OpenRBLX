#ifndef _CACHEABLECONTENTPROVIDER_
#define _CACHEABLECONTENTPROVIDER_

#include "instance.h"

typedef struct CacheableContentProvider {
    Instance instance;
} CacheableContentProvider;

CacheableContentProvider *CacheableContentProvider_new(const char *className, Instance *parent);

#endif
