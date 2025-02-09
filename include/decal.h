#ifndef _DECAL_
#define _DECAL_

#include "faceinstance.h"
#include <raylib.h>

typedef struct Decal {
    FaceInstance faceinstance;

    float Transparency;
    const char *Texture;
    float Shiny;
    float Specular;
    Color3 Color3;

    Texture2D texture;
} Decal;

Decal *Decal_new(const char *className, Instance *parent);

void serialize_Decal(Decal *decal, SerializeInstance *inst);

#endif
