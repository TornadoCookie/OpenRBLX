#include "model.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

DEFAULT_DEBUG_CHANNEL(model)

void model_draw(Model_Instance *mdl)
{

}

Model_Instance *Model_new(const char *className, Instance *parent)
{
    Model_Instance *newInst = PVInstance_new(className, parent);

    newInst->pvinstance.instance.DataCost = sizeof(Model_Instance);
    newInst->pvinstance.drawFunc = model_draw;

    newInst = realloc(newInst, sizeof(Model_Instance));
    newInst->PrimaryPart = NULL;

    if (!strcmp(newInst->pvinstance.instance.ClassName, "Model") && parent) Instance_SetParent(newInst, parent);

    return newInst;
}

void Model_BreakJoints(Model_Instance *this)
{
    FIXME("this %p stub!\n", this);
}

CFrame Model_GetModelCFrame(Model_Instance *this)
{
    if (this->PrimaryPart) return this->PrimaryPart->CFrame;

    CFrame sum = { 0 };
    int size = 0;

    BasePart *x = Instance_FindFirstChildWhichIsA(this, "BasePart", true);

    while (x)
    {
        size++;
        CFrame new = x->CFrame;
        sum.X += new.X;
        sum.Y += new.Y;
        sum.Z += new.Z;
        sum.R00 += new.R00;
        sum.R01 += new.R01;
        sum.R02 += new.R02;
        sum.R10 += new.R10;
        sum.R11 += new.R11;
        sum.R12 += new.R12;
        sum.R20 += new.R20;
        sum.R21 += new.R21;
        sum.R22 += new.R22;
        x = Instance_FindNextChildWhichIsA(this, "BasePart", x, true);
    }

    printf("cframe sum %s.\n", debugstr_cframe(sum));

    sum.X /= size;
    sum.Y /= size;
    sum.Z /= size;
    sum.R00 /= size;
    sum.R01 /= size;
    sum.R02 /= size;
    sum.R10 /= size;
    sum.R11 /= size;
    sum.R12 /= size;
    sum.R20 /= size;
    sum.R21 /= size;
    sum.R22 /= size;

    printf("cframe result %s.\n", debugstr_cframe(sum));

    return sum;
}

Vector3 Model_GetSize(Model_Instance *this)
{
    FIXME("this %p stub!\n", this);
    return (Vector3){0};
}

void Model_MakeJoints(Model_Instance *this)
{
    FIXME("this %p stub!\n", this);
}

void Model_MoveTo(Model_Instance *this, Vector3 position)
{
    CFrame cf = Model_GetModelCFrame(this);
    Vector3 delta = Vector3Subtract((Vector3){cf.X, cf.Y, cf.Z}, position);
    Model_TranslateBy(this, delta);
}

void Model_TranslateBy(Model_Instance *this, Vector3 delta)
{
    Instance **children;
    int childCount;

    children = Instance_GetChildren(this, &childCount);

    if (Instance_IsA(this, "BasePart")) BasePart_SetPosition(this, Vector3Add(((BasePart*)this)->Position, delta));

    for (int i = 0; i < childCount; i++)
    {
        Model_TranslateBy(children[i], delta);
    }

}
