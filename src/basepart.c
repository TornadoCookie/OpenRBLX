#include "basepart.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>

BasePart *BasePart_new(const char *className, Instance *parent)
{
    BasePart *newInst = PVInstance_new(className, parent);

    newInst->pvinstance.instance.DataCost = sizeof(BasePart);
    newInst = realloc(newInst, sizeof(BasePart));

    newInst->Touched = RBXScriptSignal_new();
    newInst->TouchEnded = RBXScriptSignal_new();

    return newInst;
}

void BasePart_BreakJoints(BasePart *this)
{
    FIXME("this %p stub!\n", this);
}

float BasePart_GetMass(BasePart *this)
{
    FIXME("this %p stub!\n", this);
    return 0.0f;
}

void BasePart_MakeJoints(BasePart *this)
{
    FIXME("this %p stub!\n", this);
}

bool BasePart_Resize(BasePart *this, int normalId, int deltaAmount)
{
    FIXME("this %p, normalId %d, deltaAmount %d stub!\n", this, normalId, deltaAmount);
    return true;
}
