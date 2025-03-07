#ifndef _PART_
#define _PART_

#include <raylib.h>
#include "formfactorpart.h"
#include "texturecontentprovider.h"

typedef enum {
    Shape_Ball,
    Shape_Block,
    Shape_Cylinder,
    Shape_Wedge,
    Shape_CornerWedge,
} Shape;

typedef struct Part {
    FormFactorPart formfactorpart;
    Shape shape;
    Mesh mesh;
    Material material;
    TextureContentProvider *texCont; // We cache the value of this as ServiceProvider_GetService is costly per frame
} Part;

Part *Part_new(const char *className, Instance *parent);

void Part_SetShape(Part *this, Shape shape);

void serialize_Part(Part *part, SerializeInstance *inst);

#endif
