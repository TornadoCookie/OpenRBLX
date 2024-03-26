#include "cacheablecontentprovider.h"
#include <stdlib.h>

CacheableContentProvider *CacheableContentProvider_new(const char *className, Instance *parent)
{
    CacheableContentProvider *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(CacheableContentProvider);
    newInst = realloc(newInst, sizeof(CacheableContentProvider));

    return newInst;
}
