#ifndef _PART_
#define _PART_

#include "formfactorpart.h"

typedef enum {
    Shape_Ball,
    Shape_Cylinder,
    Shape_Block,
    Shape_Wedge,
    Shape_CornerWedge,
} Shape;

typedef struct Part {
    FormFactorPart formfactorpart;
    Shape shape;
} Part;

Part *Part_new(Instance *parent);

#endif
