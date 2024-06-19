#ifndef _BEVELMESH_
#define _BEVELMESH_

#include "datamodelmesh.h"

typedef struct BevelMesh {
    DataModelMesh datamodelmesh;

    float Bevel;
    float BevelRoundness;
    float Bulge;
} BevelMesh;

BevelMesh *BevelMesh_new(const char *className, Instance *parent);

void serialize_BevelMesh(BevelMesh *bevelmesh, SerializeInstance *inst);

#endif
