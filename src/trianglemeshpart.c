#include "trianglemeshpart.h"

TriangleMeshPart *TriangleMeshPart_new(const char *className, Instance *parent)
{
    TriangleMeshPart *newInst = BasePart_new(className, parent);

    newInst = realloc(newInst, sizeof(TriangleMeshPart));
    newInst->basepart.pvinstance.instance.DataCost = sizeof(TriangleMeshPart);

    newInst->CollisionFidelity = CollisionFidelity_Default;

    return newInst;
}

void serialize_TriangleMeshPart(TriangleMeshPart *trianglemeshpart, SerializeInstance *inst)
{
    serialize_BasePart(trianglemeshpart, inst);

    serialize_atomic(token, trianglemeshpart, CollisionFidelity);
}