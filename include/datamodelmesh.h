#ifndef _DATAMODELMESH_
#define _DATAMODELMESH_

#include "instance.h"

typedef struct DataModelMesh {
    Instance instance;

    Vector3 Offset;
    Vector3 Scale;
    Vector3 VertexColor;
} DataModelMesh;

DataModelMesh *DataModelMesh_new(const char *className, Instance *parent);

void serialize_DataModelMesh(DataModelMesh *datamodelmesh, SerializeInstance *inst);

#endif
