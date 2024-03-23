#ifndef _BASEPART_
#define _BASEPART_

#include "pvinstance.h"
#include "faces.h"
#include "color3.h"

typedef struct BasePart {
    PVInstance pvinstance;

    bool Anchored;
    float BackParamA;
    float BackParamB;
    int BackSurface;
    int BacksurfaceInput;
    float BottomParamA;
    float BottomParamB;
    int BottomSurface;
    int BottomSurfaceInput;
    int BrickColor;
    CFrame CFrame;
    bool CanCollide;
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
    Vector3 Size;
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

#endif
