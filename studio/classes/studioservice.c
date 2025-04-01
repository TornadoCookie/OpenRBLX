#include "studio/studioservice.h"

StudioService *StudioService_new(const char *className, Instance *parent)
{
    StudioService *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    if (parent)
        Instance_SetParent(newInst, parent);
    
    return newInst;
}

void serialize_StudioService(StudioService *studioservice, SerializeInstance *inst)
{
    serialize_Instance(studioservice, inst);
}

