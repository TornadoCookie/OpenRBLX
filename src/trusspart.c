#include <raylib.h>
#include "trusspart.h"
#include <stdio.h>
#include "debug.h"

static Color rl_from_color3(Color3 col)
{
    return (Color){col.R * 255, col.G * 255, col.B * 255, 255};
}

void trusspart_draw(TrussPart *this)
{
    printf("draw truss part ");
    printf("at position %s ", debugstr_vector3(this->basepart.Position));
    printf("and size %f.\n", this->basepart.size.x);
    DrawSphere(
        this->basepart.Position,
        this->basepart.size.x,
        rl_from_color3(this->basepart.Color)
    );
}

TrussPart *TrussPart_new(const char *className, Instance *parent)
{
    TrussPart *newInst = BasePart_new("TrussPart", parent);

    newInst->basepart.pvinstance.instance.DataCost = sizeof(TrussPart);
    newInst->basepart.pvinstance.drawFunc = trusspart_draw;

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}
