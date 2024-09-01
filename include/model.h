#ifndef _MODEL_
#define _MODEL_

#include "pvinstance.h"
#include "basepart.h"
#include "cframe.h"
#include <raymath.h>

typedef struct ModelBoundingBox {
    CFrame orientation;
    Vector3 size;
} ModelBoundingBox;

typedef struct Model_Instance {
    PVInstance pvinstance;

    BasePart *PrimaryPart;
    char *ModelMeshData;
    CFrame WorldPivot;
} Model_Instance;

Model_Instance *Model_new(const char *className, Instance *parent);

void Model_BreakJoints(Model_Instance *this);
CFrame Model_GetModelCFrame(Model_Instance *this);
Vector3 Model_GetModelSize(Model_Instance *this);
ModelBoundingBox Model_GetBoundingBox(Model_Instance *this);
void Model_MakeJoints(Model_Instance *this);
void Model_MoveTo(Model_Instance *this, Vector3 position);
void Model_TranslateBy(Model_Instance *this, Vector3 delta);

void serialize_Model(Model_Instance *model, SerializeInstance *inst);

#endif
