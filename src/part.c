
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

static Matrix rl_from_cframe_and_size(CFrame cf, Vector3 size)
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
    return MatrixMultiply(MatrixMultiply(MatrixScale(size.x, size.y, size.z), rotated), MatrixTranslate(cf.X, cf.Y, cf.Z));
}

void part_draw(Part *this)
{
    printf("drawing part %p.\n", this);
    this->material.maps[MATERIAL_MAP_DIFFUSE].color = rl_from_color3(this->formfactorpart.basepart.Color);
    DrawMesh(this->mesh, this->material, rl_from_cframe_and_size(this->formfactorpart.basepart.CFrame, this->formfactorpart.basepart.size));

    if (this->shape == Shape_Block)
    {
        if (this->formfactorpart.basepart.TopSurface == SurfaceType_Bumps)
        {
            printf("Draw Bumps\n");
            for (int x = 0; x < this->formfactorpart.basepart.size.x; x++)
            {
                for (int z = 0; z < this->formfactorpart.basepart.size.z; z++)
                {
                    DrawCube((Vector3){x + this->formfactorpart.basepart.Position.x + 0.5f, this->formfactorpart.basepart.size.y + this->formfactorpart.basepart.Position.y, z + this->formfactorpart.basepart.Position.z + 0.5f}, 0.9, 0.9f, 0.9f, rl_from_color3(this->formfactorpart.basepart.Color));
                }
            }
        }
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
