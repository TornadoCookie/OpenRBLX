#include "localizationservice.h"

LocalizationService *LocalizationService_new(const char *className, Instance *parent)
{
    INSTANCE_STUB_CONSTRUCTOR(LocalizationService, Instance);
}

void serialize_LocalizationService(LocalizationService *localizationservice, SerializeInstance *inst)
{
    serialize_Instance(localizationservice, inst);
}

