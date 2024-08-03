#ifndef _TRIANGLEMESHPART_
#define _TRIANGLEMESHPART_

#include "basepart.h"

typedef enum {
    CollisionFidelity_Default,
    CollisionFidelity_Hull,
    CollisionFidelity_Box,
    CollisionFidelity_PreciseConvexDecomposition,
} CollisionFidelity;

typedef struct TriangleMeshPart {
    BasePart basepart;

    CollisionFidelity CollisionFidelity;
} TriangleMeshPart;

TriangleMeshPart *TriangleMeshPart_new(const char *className, Instance *parent);

void serialize_TriangleMeshPart(TriangleMeshPart *trianglemeshpart, SerializeInstance *inst);

#endif
