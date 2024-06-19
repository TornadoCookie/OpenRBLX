#ifndef _FACEINSTANCE_
#define _FACEINSTANCE_

#include "instance.h"
#include "basepart.h"

typedef struct FaceInstance {
    Instance instance;

    NormalId Face;
} FaceInstance;

FaceInstance *FaceInstance_new(const char *className, Instance *parent);

void serialize_FaceInstance(FaceInstance *faceinstance, SerializeInstance *inst);

#endif
