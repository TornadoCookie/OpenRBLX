#ifndef _STUDIOSERVICE_
#define _STUDIOSERVICE_

#include "instance.h"

typedef struct StudioService {
    Instance instance;

    const char *StudioLocaleId;
} StudioService;

StudioService *StudioService_new(const char *className, Instance *parent);
void serialize_StudioService(StudioService *studioservice, SerializeInstance *inst);

#endif

