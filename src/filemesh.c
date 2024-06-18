#include "filemesh.h"

FileMesh *FileMesh_new(const char *className, Instance *parent)
{
    FileMesh *newInst = DataModelMesh_new(className, parent);

    newInst->datamodelmesh.instance.DataCost = sizeof(FileMesh);
    newInst = realloc(newInst, sizeof(FileMesh));

    return newInst;
}

void serialize_FileMesh(FileMesh *filemesh, SerializeInstance *inst)
{
    serialize_DataModelMesh(filemesh, inst);

    serialize_atomic(string, filemesh, MeshId);
    serialize_atomic(string, filemesh, TextureId);
}
