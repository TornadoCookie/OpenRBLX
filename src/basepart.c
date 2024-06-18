#include "basepart.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include "brickcolor.h"
#include "serialize.h"
#include <raylib.h>
#include "datamodelmesh.h"
#include <string.h>

DEFAULT_DEBUG_CHANNEL(basepart)

static Color rl_from_color3(Color3 col, float transparency)
{
    return (Color){col.R * 255, col.G * 255, col.B * 255, (1.0f-transparency) * 255};
}

BasePart *BasePart_new(const char *className, Instance *parent)
{
    BasePart *newInst = PVInstance_new(className, parent);

    newInst->pvinstance.instance.DataCost = sizeof(BasePart);
    newInst = realloc(newInst, sizeof(BasePart));

    newInst->Touched = RBXScriptSignal_new();
    newInst->TouchEnded = RBXScriptSignal_new();

    newInst->pvinstance.drawFunc = BasePart_Draw;

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

void BasePart_SetColor(BasePart *this, Color3 color)
{
    this->Color = color;
    this->BrickColor = GetBrickColorFromColor(color).paletteIndex;
}

void BasePart_SetBrickColor(BasePart *this, int brickColor)
{
    this->BrickColor = brickColor;
    this->Color = GetBrickColorFromId(brickColor).color;
}

void BasePart_SetCFrame(BasePart *this, CFrame cf)
{
    this->CFrame = cf;
    this->Position = (Vector3){cf.X, cf.Y, cf.Z};
}

void BasePart_SetPosition(BasePart *this, Vector3 pos)
{
    this->CFrame.X = pos.x;
    this->CFrame.Y = pos.y;
    this->CFrame.Z = pos.z;
    this->Position = pos;
}

void BasePart_Draw(BasePart *this)
{
    int childCount;
    Instance **children = Instance_GetChildren(this, &childCount);

    for (int i = 0; i < childCount; i++)
    {
        Instance *child = children[i];
        if (!strcmp(child->ClassName, "CylinderMesh"))
        {
            DataModelMesh *dmmesh = child;
            Color3 vertexColor = this->Color;
            DrawCylinder(
                Vector3Add(this->Position, dmmesh->Offset),
                this->size.x + dmmesh->Scale.x,
                this->size.x + dmmesh->Scale.x,
                this->size.y + dmmesh->Scale.y,
                12, rl_from_color3(vertexColor, 0.0f));
        }
    }
}

void serialize_BasePart(BasePart *basepart, SerializeInstance *inst)
{
    serialize_PVInstance(basepart, inst);

    serialize_atomic(bool, basepart, Anchored);
    serialize_atomic(float, basepart, BackParamA);
    serialize_atomic(float, basepart, BackParamB);
    serialize_atomic(token, basepart, BackSurface);
    serialize_atomic(token, basepart, BackSurfaceInput);
    serialize_atomic(float, basepart, BottomParamA);
    serialize_atomic(float, basepart, BottomParamB);
    serialize_atomic(token, basepart, BottomSurface);
    serialize_atomic(token, basepart, BottomSurfaceInput);
    serialize_atomic(int, basepart, BrickColor);
    serialize_atomic(CoordinateFrame, basepart, CFrame);
    serialize_atomic(bool, basepart, CanCollide);
    serialize_atomic(bool, basepart, DraggingV1);
    serialize_atomic(float, basepart, Elasticity);
    serialize_atomic(float, basepart, Friction);
    serialize_atomic(float, basepart, FrontParamA);
    serialize_atomic(float, basepart, FrontParamB);
    serialize_atomic(token, basepart, FrontSurface);
    serialize_atomic(token, basepart, FrontSurfaceInput);
    serialize_atomic(float, basepart, LeftParamA);
    serialize_atomic(float, basepart, LeftParamB);
    serialize_atomic(token, basepart, LeftSurface);
    serialize_atomic(token, basepart, LeftSurfaceInput);
    serialize_atomic(bool, basepart, Locked);
    serialize_atomic(token, basepart, Material);
    serialize_atomic(float, basepart, Reflectance);
    serialize_atomic(float, basepart, RightParamA);
    serialize_atomic(float, basepart, RightParamB);
    serialize_atomic(token, basepart, RightSurface);
    serialize_atomic(token, basepart, RightSurfaceInput);
    serialize_atomic(Vector3, basepart, RotVelocity);
    serialize_atomic(float, basepart, TopParamA);
    serialize_atomic(float, basepart, TopParamB);
    serialize_atomic(token, basepart, TopSurface);
    serialize_atomic(token, basepart, TopSurfaceInput);
    serialize_atomic(float, basepart, Transparency);
    serialize_atomic(Vector3, basepart, Velocity);
    serialize_atomic(Vector3, basepart, size);
    serialize_atomic(Color3, basepart, Color);

    serialize_atomic(token, inst, TopConstraint);
    serialize_atomic(token, inst, BottomConstraint);
    serialize_atomic(token, inst, LeftConstraint);
    serialize_atomic(token, inst, RightConstraint);
    serialize_atomic(token, inst, FrontConstraint);
    serialize_atomic(token, inst, BackConstraint);
    serialize_atomic(int, inst, Color3uint8);
}
