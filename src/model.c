#include "model.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>

Model_Instance *Model_new(const char *className, Instance *parent)
{
    Model_Instance *newInst = PVInstance_new(className, parent);

    newInst->pvinstance.instance.DataCost = sizeof(Model_Instance);

    if (!strcmp(newInst->pvinstance.instance.ClassName, "Model") && parent) Instance_SetParent(newInst, parent);

    return newInst;
}

void Model_BreakJoints(Model_Instance *this)
{
    FIXME("this %p stub!\n", this);
}

CFrame Model_GetModelCFrame(Model_Instance *this)
{
    FIXME("this %p stub!\n", this);
    return (CFrame){0};
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
    FIXME("this %p, position %s stub!\n", this, debugstr_vector3(position));
}

void Model_TranslateBy(Model_Instance *this, Vector3 delta)
{
    FIXME("this %p, delta %s stub!\n", this, debugstr_vector3(delta));
}
