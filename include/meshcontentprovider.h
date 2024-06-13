#ifndef _MESHCONTENTPROVIDER_
#define _MESHCONTENTPROVIDER_

#include <raylib.h>
#include "cacheablecontentprovider.h"
#include "part.h"

typedef struct MeshContentProvider {
    CacheableContentProvider cacheablecontentprovider;

    Mesh ballMesh;
    Mesh cylinderMesh;
    Mesh blockMesh;
    Texture2D studs;
} MeshContentProvider;

MeshContentProvider *MeshContentProvider_new(const char *className, Instance *parent);

Mesh MeshContentProvider_GetPartMesh(MeshContentProvider *this, Shape shape);

#endif
