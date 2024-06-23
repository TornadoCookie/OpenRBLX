#ifndef _FILEMESH_
#define _FILEMESH_

#include "datamodelmesh.h"
#include <raylib.h>

typedef struct FileMesh {
    DataModelMesh datamodelmesh;

    const char *MeshId;
    const char *TextureId;
    Mesh mesh;
    bool meshLoaded;
    Material material;
} FileMesh;

FileMesh *FileMesh_new(const char *className, Instance *parent);

void FileMesh_Draw(FileMesh *this, BasePart *part);

void serialize_FileMesh(FileMesh *filemesh, SerializeInstance *inst);

#endif

