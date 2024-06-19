#include "faceinstance.h"

FaceInstance *FaceInstance_new(const char *className, Instance *parent)
{
    FaceInstance *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(FaceInstance);
    newInst = realloc(newInst, sizeof(FaceInstance));

    return newInst;
}

void serialize_FaceInstance(FaceInstance *faceinstance, SerializeInstance *inst)
{
    serialize_Instance(faceinstance, inst);

    serialize_atomic(token, faceinstance, Face);
}
