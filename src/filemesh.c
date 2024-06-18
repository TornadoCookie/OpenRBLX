#include "filemesh.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(filemesh)

void FileMesh_Draw(FileMesh *this, BasePart *part)
{
    FIXME("No way to get content: %s.\n", this->MeshId);
}

FileMesh *FileMesh_new(const char *className, Instance *parent)
{
    FileMesh *newInst = DataModelMesh_new(className, parent);

    newInst->datamodelmesh.instance.DataCost = sizeof(FileMesh);
    newInst = realloc(newInst, sizeof(FileMesh));

    newInst->datamodelmesh.drawFunc = FileMesh_Draw;

    return newInst;
}

void serialize_FileMesh(FileMesh *filemesh, SerializeInstance *inst)
{
    serialize_DataModelMesh(filemesh, inst);

    serialize_atomic(string, filemesh, MeshId);
    serialize_atomic(string, filemesh, TextureId);
}
