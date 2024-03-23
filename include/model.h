#ifndef _MODEL_
#define _MODEL_

#include "pvinstance.h"
#include "basepart.h"
#include "cframe.h"
#include <raymath.h>

typedef struct Model {
    PVInstance pvinstance;

    BasePart *PrimaryPart;
} Model;

Model *Model_new(const char *className, Instance *parent);

void Model_BreakJoints(Model *this);
CFrame Model_GetModelCFrame(Model *this);
Vector3 Model_GetSize(Model *this);
void Model_MakeJoints(Model *this);
void Model_MoveTo(Model *this, Vector3 position);
void Model_TranslateBy(Model *this, Vector3 delta);

#endif
