#ifndef _COLLECTIONSERVICE_
#define _COLLECTIONSERVICE_

#include "instance.h"

typedef struct CollectionService {
    Instance instance;
} CollectionService;

CollectionService *CollectionService_new(const char *className, Instance *parent);

#endif

