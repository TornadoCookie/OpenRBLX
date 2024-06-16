#include <raylib.h>
#include <rlgl.h>
#include "wedgepart.h"
#include "meshcontentprovider.h"
#include "serviceprovider.h"
#include "datamodel.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(wedgepart)

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

static void wedgepart_draw(WedgePart *this)
{
    
    this->material.maps[MATERIAL_MAP_DIFFUSE].color = rl_from_color3(this->formfactorpart.basepart.Color, this->formfactorpart.basepart.Transparency);

    CFrame cf = this->formfactorpart.basepart.CFrame;

    DrawMesh(this->mesh, this->material, cf_size_to_matrix(cf, this->formfactorpart.basepart.size));

}

WedgePart *WedgePart_new(const char *className, Instance *parent)
{
    WedgePart *newInst = FormFactorPart_new("WedgePart", parent);

    newInst->formfactorpart.basepart.pvinstance.instance.DataCost = sizeof(WedgePart);
    newInst = realloc(newInst, sizeof(WedgePart));

    newInst->formfactorpart.basepart.pvinstance.drawFunc = wedgepart_draw;

    if (parent) Instance_SetParent(newInst, parent);

    MeshContentProvider *mcp = ServiceProvider_GetService(GetDataModel(), "MeshContentProvider");
    newInst->mesh = mcp->wedgeMesh;
    newInst->material = LoadMaterialDefault();

    return newInst;
}

void serialize_WedgePart(WedgePart *wedgepart, SerializeInstance *inst)
{
    serialize_FormFactorPart(wedgepart, inst);
}
