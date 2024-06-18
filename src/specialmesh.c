#include "specialmesh.h"

SpecialMesh *SpecialMesh_new(const char *className, Instance *parent)
{
    SpecialMesh *newInst = FileMesh_new("SpecialMesh", parent);

    newInst->filemesh.datamodelmesh.instance.DataCost = sizeof(SpecialMesh);
    newInst = realloc(newInst, sizeof(SpecialMesh));

    newInst->MeshType = MeshType_Head;

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

void serialize_SpecialMesh(SpecialMesh *specialmesh, SerializeInstance *inst)
{
    serialize_FileMesh(specialmesh, inst);
    
    serialize_atomic(token, specialmesh, MeshType);
}
