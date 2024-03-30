
#include <raylib.h>
#include "part.h"
#include <stdlib.h>
#include "debug.h"
#include <stdio.h>
#include "meshcontentprovider.h"
#include "datamodel.h"

DEFAULT_DEBUG_CHANNEL(part)

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

static void draw_block(Part *this)
{
    printf("position %s.\n", debugstr_vector3(this->formfactorpart.basepart.Position));
    DrawMesh(this->mesh, this->material, cf_size_to_matrix(this->formfactorpart.basepart.CFrame, this->formfactorpart.basepart.size));
}

static void draw_cylinder(Part *this)
{
    CFrame cf = this->formfactorpart.basepart.CFrame;
    float r00 = cf.R00, r10 = cf.R10, r20 = cf.R20;
    cf.R00 = cf.R01;
    cf.R10 = cf.R11;
    cf.R20 = cf.R21;
    cf.R01 = r00;
    cf.R11 = r10;
    cf.R21 = r20;
    cf.R02 = -cf.R02;
    cf.R12 = -cf.R12;
    cf.R22 = -cf.R22;
    cf.X -= this->formfactorpart.basepart.size.x / 2;
    cf.Y += this->formfactorpart.basepart.size.y / 4;
    cf.Z -= this->formfactorpart.basepart.size.z;
    DrawMesh(this->mesh, this->material, cf_size_to_matrix(cf, this->formfactorpart.basepart.size));
}

static void draw_ball(Part *this)
{
    CFrame cf = this->formfactorpart.basepart.CFrame;
    cf.X += this->formfactorpart.basepart.size.x / 2;
    cf.Z += this->formfactorpart.basepart.size.z / 2;
    DrawMesh(this->mesh, this->material, cf_size_to_matrix(cf, this->formfactorpart.basepart.size));
}

void part_draw(Part *this)
{
    printf("drawing part %p.\n", this);
    this->material.maps[MATERIAL_MAP_DIFFUSE].color = rl_from_color3(this->formfactorpart.basepart.Color, this->formfactorpart.basepart.Transparency);

    switch (this->shape)
    {
        case Shape_Block: draw_block(this); break;
        case Shape_Cylinder: draw_cylinder(this); break;
        case Shape_Ball: draw_ball(this); break;
        default: FIXME("not implemented: %d.\n", this->shape); break;
    }
}

Part *Part_new(Instance *parent)
{
    Part *newInst = FormFactorPart_new("Part", parent);

    newInst->formfactorpart.basepart.pvinstance.instance.DataCost = sizeof(Part);
    newInst = realloc(newInst, sizeof(Part));

    newInst->formfactorpart.basepart.pvinstance.drawFunc = part_draw;

    newInst->material = LoadMaterialDefault();
    newInst->shape = Shape_Block;

    MeshContentProvider *mcp = ServiceProvider_GetService(GetDataModel(), "MeshContentProvider");

    //newInst->material.maps[MATERIAL_MAP_DIFFUSE].texture = mcp->studs;

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

void Part_SetShape(Part *this, Shape shape)
{
    MeshContentProvider *mcp = ServiceProvider_GetService(GetDataModel(), "MeshContentProvider");
    this->mesh = MeshContentProvider_GetPartMesh(mcp, this->shape);
}
