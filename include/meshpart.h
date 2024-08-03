#ifndef _MESHPART_
#define _MESHPART_

#include "trianglemeshpart.h"
#include <raylib.h>

typedef struct MeshPart {
    TriangleMeshPart trianglemeshpart;

    const char *MeshId;
    const char *TextureID;
    Mesh mesh;
    bool meshLoaded;
    Material material;
} MeshPart;

MeshPart *MeshPart_new(const char *className, Instance *parent);

void MeshPart_Draw(MeshPart *this);

void serialize_MeshPart(MeshPart *meshpart, SerializeInstance *inst);

#endif
