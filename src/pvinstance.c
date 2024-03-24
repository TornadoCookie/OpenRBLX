#include "pvinstance.h"
#include <stdio.h>
#include <stdlib.h>

void pvinstance_draw(PVInstance *this)
{
    printf("warning: classname %s has no draw func\n", this->instance.ClassName);
}

PVInstance *PVInstance_new(const char *className, Instance *parent)
{
    PVInstance *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(PVInstance);
    newInst = realloc(newInst, sizeof(PVInstance));

    newInst->drawFunc = pvinstance_draw;

    return newInst;
}

void PVInstance_Draw(PVInstance *this)
{
    this->drawFunc(this);
}
