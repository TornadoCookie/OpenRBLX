#include "meshcontentprovider.h"
#include <raylib.h>
#include <stdlib.h>
#include "debug.h"
#include <rlgl.h>

DEFAULT_DEBUG_CHANNEL(meshcontentprovider)

MeshContentProvider *MeshContentProvider_new(Instance *parent)
{
    MeshContentProvider *newInst = CacheableContentProvider_new("MeshContentProvider", parent);

    newInst->cacheablecontentprovider.instance.DataCost = sizeof(MeshContentProvider);
    newInst = realloc(newInst, sizeof(MeshContentProvider));

    newInst->ballMesh = GenMeshSphere(1.0f, 12, 24);
    newInst->blockMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    newInst->cylinderMesh = GenMeshCylinder(0.5f, 1.0f, 24);

    newInst->studs = LoadTexture("staticdata/studs.png");

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

Mesh MeshContentProvider_GetPartMesh(MeshContentProvider *this, Shape shape)
{
    switch (shape)
    {
        case Shape_Ball: return this->ballMesh;
        case Shape_Block: return this->blockMesh;
        case Shape_Cylinder: return this->cylinderMesh;
        default: FIXME("shape %d not implemented.\n", shape); break;
    }
}
