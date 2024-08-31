#ifndef _CACHEABLECONTENTPROVIDER_
#define _CACHEABLECONTENTPROVIDER_

#include "instance.h"

typedef struct CacheableContentProvider {
    Instance instance;
} CacheableContentProvider;

CacheableContentProvider *CacheableContentProvider_new(const char *className, Instance *parent);

const char *CacheableContentProvider_LoadAssetDelivery(CacheableContentProvider *this, long assetId, int *dataSize);
long CacheableContentProvider_GetAssetIdFromContent(CacheableContentProvider *this, const char *content);

#endif
