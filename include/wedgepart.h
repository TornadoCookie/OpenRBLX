#ifndef _WEDGEPART_
#define _WEDGEPART_

#include "formfactorpart.h"
#include <raylib.h>

typedef struct WedgePart {
    FormFactorPart formfactorpart;
    Mesh mesh;
    Material material;
} WedgePart;

WedgePart *WedgePart_new(const char *className, Instance *parent);

void serialize_WedgePart(WedgePart *wedgepart, SerializeInstance *inst);

#endif
