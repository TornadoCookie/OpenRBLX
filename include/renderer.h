#ifndef _RENDERER_
#define _RENDERER_

#include "basepart.h"

// MUST BE ALIGNED WITH enum Shape in include/part.h
typedef enum {
    RS_Ball,
    RS_Block,
    RS_Cylinder,
    RS_Wedge,
    RS_CornerWedge,
    RS_Mesh,
} RenderShape;

typedef struct RenderObject3D {
    RenderShape shape;
    const char *meshId;
    const char *meshTextureId;
    Color3 color;
    float transparency;
    CFrame cframe;
    Vector3 size;
} RenderObject3D;

void InitRenderer();

void RenderGame3D();

// Add a render object to the current frame
void Add3DRenderObject(RenderObject3D obj);

#endif

