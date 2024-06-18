#ifndef _FILEMESH_
#define _FILEMESH_

#include "datamodelmesh.h"

typedef struct FileMesh {
    DataModelMesh datamodelmesh;

    const char *MeshId;
    const char *TextureId;
} FileMesh;

FileMesh *FileMesh_new(const char *className, Instance *parent);

void FileMesh_Draw(FileMesh *this, BasePart *part);

void serialize_FileMesh(FileMesh *filemesh, SerializeInstance *inst);

#endif

