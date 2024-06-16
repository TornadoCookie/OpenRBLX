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
    CFrame cf = Model_GetModelCFrame(mdl);
    DrawCube((Vector3){cf.X, cf.Y, cf.Z}, 1, 1, 1, WHITE);
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

static BoundingBox GetModelBoundingBoxNoTransform(Model model, Vector3 *positions)
{
    BoundingBox bounds = { 0 };

    if (model.meshCount > 0)
    {
        Vector3 temp = { 0 };
        bounds = GetMeshBoundingBox(model.meshes[0]);

        for (int i = 1; i < model.meshCount; i++)
        {
            BoundingBox tempBounds = GetMeshBoundingBox(model.meshes[i]);
            tempBounds.max = Vector3Add(tempBounds.max, positions[i]);
            tempBounds.min = Vector3Add(tempBounds.min, positions[i]);

            temp.x = (bounds.min.x < tempBounds.min.x)? bounds.min.x : tempBounds.min.x;
            temp.y = (bounds.min.y < tempBounds.min.y)? bounds.min.y : tempBounds.min.y;
            temp.z = (bounds.min.z < tempBounds.min.z)? bounds.min.z : tempBounds.min.z;
            bounds.min = temp;

            temp.x = (bounds.max.x > tempBounds.max.x)? bounds.max.x : tempBounds.max.x;
            temp.y = (bounds.max.y > tempBounds.max.y)? bounds.max.y : tempBounds.max.y;
            temp.z = (bounds.max.z > tempBounds.max.z)? bounds.max.z : tempBounds.max.z;
            bounds.max = temp;
        }
    }

    return bounds;
}

ModelBoundingBox Model_GetBoundingBox(Model_Instance *this)
{
    ModelBoundingBox ret = { 0 };
    Model mdl = { 0 };

    int descendantCount = 0;
    Instance **descendants = Instance_GetDescendants(this, &descendantCount);
    Vector3 *positions = NULL;

    for (int i = 0; i < descendantCount; i++)
    {
        if (Instance_IsA(descendants[i], "Part"))
        {
            mdl.meshCount++;
            mdl.meshes = realloc(mdl.meshes, mdl.meshCount * sizeof(Mesh));
            mdl.meshes[mdl.meshCount - 1] = ((Part*)descendants[i])->mesh;
            positions = realloc(positions, mdl.meshCount * sizeof(Vector3));
            positions[mdl.meshCount - 1] = ((Part*)descendants[i])->formfactorpart.basepart.Position;
        }
    }

    BoundingBox box = GetModelBoundingBoxNoTransform(mdl, positions);

    free(mdl.meshes);
    free(descendants);

    ret.size = Vector3Subtract(box.max, box.min);
    Vector3 position = Vector3Lerp(box.min, box.max, 0.5f);
    //printf("max = %s, min = %s.\n", debugstr_vector3(box.max), debugstr_vector3(box.min));

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

void serialize_Model(Model_Instance *model, SerializeInstance *inst)
{
    serialize_PVInstance(model, inst);

    _serialize_atomic(inst, (Serialization){Serialize_CoordinateFrame, "CoordinateFrame", &inst->modelOffset});
}
