
#include <raylib.h>
#include "part.h"
#include <stdlib.h>
#include "debug.h"
#include <stdio.h>
#include "meshcontentprovider.h"
#include "datamodel.h"

DEFAULT_DEBUG_CHANNEL(part)

static Color rl_from_color3(Color3 col)
{
    return (Color){col.R * 255, col.G * 255, col.B * 255, 255};
}

static Matrix rl_from_cframe_and_size(CFrame cf, Vector3 size, Shape shape)
{
    Matrix rotated = {
        .m0 = -cf.R00,
        .m1 = -cf.R10,
        .m2 = -cf.R20,
        .m4 = cf.R01,
        .m5 = cf.R11,
        .m6 = cf.R21,
        .m8 = -cf.R02,
        .m9 = -cf.R12,
        .m10 = -cf.R22,
        .m15 = 1.0f,
    };
    printf("rotated = %s.\n", debugstr_cframe(cf));
    Vector3 scale = size;
    if (shape == Shape_Cylinder)
    {
        float sizeX = scale.y;
        scale.y = scale.z;
        scale.z = sizeX;

        rotated = (Matrix) {
            .m4 = -cf.R00,
            .m5 = -cf.R10,
            .m6 = -cf.R20,
            .m0 = -cf.R01,
            .m1 = -cf.R11,
            .m2 = -cf.R21,
            .m8 = -cf.R02,
            .m9 = -cf.R12,
            .m10 = -cf.R22,
            .m15 = 1.0f,
        };
    }
    Vector3 translate = (Vector3){cf.X, cf.Y, cf.Z};
    if (shape == Shape_Ball)
    {
        translate = Vector3Add(translate, Vector3Divide(scale, (Vector3){2, 2, 2}));
    }
    return MatrixMultiply(MatrixMultiply(MatrixScale(scale.x, scale.y, scale.z), rotated), MatrixTranslate(translate.x, translate.y, translate.z));
}

void part_draw(Part *this)
{
    printf("drawing part %p.\n", this);
    this->material.maps[MATERIAL_MAP_DIFFUSE].color = rl_from_color3(this->formfactorpart.basepart.Color);
    DrawMesh(this->mesh, this->material, rl_from_cframe_and_size(this->formfactorpart.basepart.CFrame, this->formfactorpart.basepart.size, this->shape));
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
