
#include <raylib.h>
#include "part.h"
#include <stdlib.h>
#include "debug.h"
#include <stdio.h>

DEFAULT_DEBUG_CHANNEL(part)

static Color rl_from_color3(Color3 col)
{
    return (Color){col.R * 255, col.G * 255, col.B * 255, 255};
}

void part_draw(Part *this)
{
    printf("drawing part %p.\n", this);
    switch (this->shape)
    {
        case Shape_Ball:
        {
            DrawSphere(
                this->formfactorpart.basepart.Position,
                this->formfactorpart.basepart.size.x,
                rl_from_color3(this->formfactorpart.basepart.Color)
            );
        } break;
        case Shape_Cylinder:
        {
            DrawCylinder(
                this->formfactorpart.basepart.Position,
                this->formfactorpart.basepart.size.x,
                this->formfactorpart.basepart.size.x,
                this->formfactorpart.basepart.size.y,
                6,
                rl_from_color3(this->formfactorpart.basepart.Color)
            );
        } break;
        case Shape_Block:
        {
            DrawCube(
                this->formfactorpart.basepart.Position,
                this->formfactorpart.basepart.size.x,
                this->formfactorpart.basepart.size.y,
                this->formfactorpart.basepart.size.z,
                rl_from_color3(this->formfactorpart.basepart.Color)
            );
        } break;
        default:
        {
            FIXME("%d draw not implemented.\n", this->shape);
        } break;
    }
}

Part *Part_new(Instance *parent)
{
    Part *newInst = FormFactorPart_new("Part", parent);

    newInst->formfactorpart.basepart.pvinstance.instance.DataCost = sizeof(Part);
    newInst = realloc(newInst, sizeof(Part));

    newInst->formfactorpart.basepart.pvinstance.drawFunc = part_draw;

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}
