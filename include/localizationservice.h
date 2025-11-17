#ifndef _LOCALIZATIONSERVICE_
#define _LOCALIZATIONSERVICE_

#include "instance.h"

typedef struct LocalizationService {
    Instance instance;
} LocalizationService;

LocalizationService *LocalizationService_new(const char *className, Instance *parent);
void serialize_LocalizationService(LocalizationService *localizationservice, SerializeInstance *inst);

#endif

