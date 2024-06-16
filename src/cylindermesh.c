#include "cylindermesh.h"

CylinderMesh *CylinderMesh_new(const char *className, Instance *parent)
{
    CylinderMesh *newInst = BevelMesh_new("CylinderMesh", parent);

    newInst->bevelmesh.datamodelmesh.instance.DataCost = sizeof(CylinderMesh);
    newInst = realloc(newInst, sizeof(CylinderMesh));

    return newInst;
}

void serialize_CylinderMesh(CylinderMesh *cylindermesh, SerializeInstance *inst)
{
    serialize_BevelMesh(cylindermesh, inst);
}
