#include "meshpart.h"
#include "debug.h"
#include "meshcontentprovider.h"
#include "datamodel.h"
#include <raymath.h>

DEFAULT_DEBUG_CHANNEL(meshpart)

MeshPart *MeshPart_new(const char *className, Instance *parent)
{
    MeshPart *newInst = TriangleMeshPart_new(className, parent);

    newInst = realloc(newInst, sizeof(MeshPart));
    newInst->trianglemeshpart.basepart.pvinstance.instance.DataCost = sizeof(MeshPart);

    if (parent) Instance_SetParent(newInst, parent);

    newInst->meshLoaded = false;
    newInst->trianglemeshpart.basepart.pvinstance.drawFunc = MeshPart_Draw;

    return newInst;
}

static Matrix cf_to_rot_matrix(CFrame cf)
{
    return (Matrix) {
        // Right to Left, Up to Up, Back to Forward
        -cf.R00, cf.R01, -cf.R02, 0.0f,
        -cf.R10, cf.R11, -cf.R12, 0.0f,
        -cf.R20, cf.R21, -cf.R22, 0.0f,
           0.0f,   0.0f,    0.0f, 1.0f };
}

static Matrix cf_to_trans_matrix(CFrame cf)
{
    return MatrixTranslate(cf.X, cf.Y, cf.Z);
}

static Matrix size_to_scale_matrix(Vector3 size)
{
    return MatrixScale(size.x, size.y, size.z);
}

static Matrix cf_size_to_matrix(CFrame cf, Vector3 size)
{
    return MatrixMultiply(MatrixMultiply(size_to_scale_matrix(size), cf_to_rot_matrix(cf)), cf_to_trans_matrix(cf));
}

void MeshPart_Draw(MeshPart *this)
{
    if (this->trianglemeshpart.CollisionFidelity)
    {
        FIXME("Ignoring CollisionFidelity %d.\n", this->trianglemeshpart.CollisionFidelity);
    }

    if (!this->meshLoaded)
    {
        this->mesh = MeshContentProvider_GetFileMesh(ServiceProvider_GetService(GetDataModel(), "MeshContentProvider"), this->MeshId);
        this->meshLoaded = true;
        this->material = LoadMaterialDefault();
    }

    DrawMesh(this->mesh, this->material, cf_size_to_matrix(this->trianglemeshpart.basepart.CFrame, this->trianglemeshpart.basepart.size));
}

void serialize_MeshPart(MeshPart *meshpart, SerializeInstance *inst)
{
    serialize_TriangleMeshPart(meshpart, inst);

    serialize_atomic(string, meshpart, MeshId);
    serialize_atomic(string, meshpart, TextureID);
}
