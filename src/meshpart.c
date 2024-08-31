#include "meshpart.h"
#include "debug.h"
#include "meshcontentprovider.h"
#include "datamodel.h"
#include <raymath.h>
#include <stdio.h>

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

static Color rl_from_color3(Color3 col, float transparency)
{
    return (Color){col.R * 255, col.G * 255, col.B * 255, (1.0f-transparency) * 255};
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
        printf("Attempt to get MeshId %s\n", this->MeshId);
        this->mesh = MeshContentProvider_GetFileMesh(ServiceProvider_GetService(GetDataModel(), "MeshContentProvider"), this->MeshId);
        this->meshLoaded = true;
        this->material = LoadMaterialDefault();
        this->material.maps[MATERIAL_MAP_DIFFUSE].texture = TextureContentProvider_LoadTextureAsset(ServiceProvider_GetService(GetDataModel(), "TextureContentProvider"), this->TextureID);
        this->material.maps[MATERIAL_MAP_DIFFUSE].color = rl_from_color3(this->trianglemeshpart.basepart.Color, this->trianglemeshpart.basepart.Transparency);
    }

    DrawMesh(this->mesh, this->material, cf_size_to_matrix(this->trianglemeshpart.basepart.CFrame, this->trianglemeshpart.basepart.size));
}

void serialize_MeshPart(MeshPart *meshpart, SerializeInstance *inst)
{
    serialize_TriangleMeshPart(meshpart, inst);

    serialize_atomic(string, meshpart, MeshId);
    serialize_atomic(string, meshpart, TextureID);
}
