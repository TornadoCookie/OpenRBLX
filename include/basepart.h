#ifndef _BASEPART_
#define _BASEPART_

#include "pvinstance.h"
#include "faces.h"
#include "color3.h"

typedef enum {
    SurfaceType_Smooth,
    SurfaceType_Glue,
    SurfaceType_Weld,
    SurfaceType_Studs,
    SurfaceType_Inlet,
    SurfaceType_Universal,
    SurfaceType_Hinge,
    SurfaceType_Motor,
    SurfaceType_SteppingMotor,

    // 2005 SurfaceTypes
    SurfaceType_Bumps,
    SurfaceType_Spawn
} SurfaceType;

typedef enum {
    Constraint05_None,
    Constraint05_Hinge,
    Constraint05_Motor,
    Constraint05_SteppingMotor,
} Constraint05;

typedef enum {
    NormalId_Right,
    NormalId_Top,
    NormalId_Back,
    NormalId_Left,
    NormalId_Bottom,
    NormalId_Front
} NormalId;

typedef struct BasePart {
    PVInstance pvinstance;

    bool Anchored;
    float BackParamA;
    float BackParamB;
    int BackSurface;
    int BackSurfaceInput;
    float BottomParamA;
    float BottomParamB;
    int BottomSurface;
    int BottomSurfaceInput;
    int BrickColor;
    CFrame CFrame;
    bool CanCollide;
    bool DraggingV1;
    Color3 Color;
    float Elasticity;
    float Friction;
    float FrontParamA;
    float FrontParamB;
    int FrontSurface;
    int FrontSurfaceInput;
    float LeftParamA;
    float LeftParamB;
    int LeftSurface;
    int LeftSurfaceInput;
    bool Locked;
    int Material;
    Vector3 Position;
    float Reflectance;
    int ResizeIncrement;
    Faces ResizableFaces;
    float RightParamA;
    float RightParamB;
    int RightSurface;
    int RightSurfaceInput;
    Vector3 RotVelocity;
    Vector3 size;
    float TopParamA;
    float TopParamB;
    int TopSurface;
    int TopSurfaceInput;
    float Transparency;
    Vector3 Velocity;

//    RBXScriptSignal *LocalSimulationTouched;
    RBXScriptSignal *OutfitChanged;
//    RBXScriptSignal *StoppedTouching;
    RBXScriptSignal *TouchEnded;
    RBXScriptSignal *Touched;
} BasePart;

BasePart *BasePart_new(const char *className, Instance *parent);

void BasePart_BreakJoints(BasePart *this);
float BasePart_GetMass(BasePart *this);
void BasePart_MakeJoints(BasePart *this);
bool BasePart_Resize(BasePart *this, int normalId, int deltaAmount);

void BasePart_SetColor(BasePart *this, Color3 color);
void BasePart_SetBrickColor(BasePart *this, int brickColor);
void BasePart_SetCFrame(BasePart *this, CFrame cf);
void BasePart_SetPosition(BasePart *this, Vector3 pos);

void serialize_BasePart(BasePart *basepart, SerializeInstance *inst);

#endif
