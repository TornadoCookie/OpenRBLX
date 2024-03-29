#include <raylib.h>
#include "model.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "part.h"

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

    return Model_GetBoundingBox(this).orientation;
}

Vector3 Model_GetModelSize(Model_Instance *this)
{
    return Model_GetBoundingBox(this).size;
}

ModelBoundingBox Model_GetBoundingBox(Model_Instance *this)
{
    ModelBoundingBox ret = { 0 };
    Model mdl = { 0 };

    int descendantCount = 0;
    Instance **descendants = Instance_GetDescendants(this, &descendantCount);

    for (int i = 0; i < descendantCount; i++)
    {
        if (Instance_IsA(descendants[i], "Part"))
        {
            mdl.meshCount++;
            mdl.meshes = realloc(mdl.meshes, mdl.meshCount * sizeof(Mesh));
            mdl.meshes[mdl.meshCount - 1] = ((Part*)descendants[i])->mesh;
        }
    }

    BoundingBox box = GetModelBoundingBox(mdl);

    free(mdl.meshes);
    free(descendants);

    ret.size = Vector3Subtract(box.max, box.min);
    Vector3 position = Vector3Lerp(box.min, box.max, 0.5f);

    ret.orientation.X = position.x;
    ret.orientation.Y = position.y;
    ret.orientation.Z = position.z;

    return ret;
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
