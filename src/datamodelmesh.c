#include "datamodelmesh.h"

DataModelMesh *DataModelMesh_new(const char *className, Instance *parent)
{
    Instance *newInst = Instance_new(className, parent);

    newInst->DataCost = sizeof(DataModelMesh);
    newInst = realloc(newInst, sizeof(DataModelMesh));

    return newInst;
}

void serialize_DataModelMesh(DataModelMesh *datamodelmesh, SerializeInstance *inst)
{
    serialize_Instance(datamodelmesh, inst);

    serialize_atomic(Vector3, datamodelmesh, Offset);
    serialize_atomic(Vector3, datamodelmesh, Scale);
    serialize_atomic(Vector3, datamodelmesh, VertexColor);
}
