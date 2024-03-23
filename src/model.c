#include "model.h"
#include "debug.h"
#include <stdio.h>

Model *Model_new(const char *className, Instance *parent)
{
    Model *newInst = PVInstance_new(className, parent);

    newInst->pvinstance.instance.DataCost = sizeof(Model);

    return newInst;
}

void Model_BreakJoints(Model *this)
{
    FIXME("this %p stub!\n", this);
}

CFrame Model_GetModelCFrame(Model *this)
{
    FIXME("this %p stub!\n", this);
    return (CFrame){0};
}

Vector3 Model_GetSize(Model *this)
{
    FIXME("this %p stub!\n", this);
    return (Vector3){0};
}

void Model_MakeJoints(Model *this)
{
    FIXME("this %p stub!\n", this);
}

void Model_MoveTo(Model *this, Vector3 position)
{
    FIXME("this %p, position %s stub!\n", this, debugstr_vector3(position));
}

void Model_TranslateBy(Model *this, Vector3 delta)
{
    FIXME("this %p, delta %s stub!\n", this, debugstr_vector3(delta));
}
