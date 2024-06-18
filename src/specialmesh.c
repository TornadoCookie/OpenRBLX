#include "specialmesh.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(specialmesh)

void draw_specialmesh(SpecialMesh *this, BasePart *part)
{
    switch (this->MeshType)
    {
        case MeshType_FileMesh:
        {
            FileMesh_Draw(this, part);
        } break;
        default:
        {
            FIXME("Unsupported MeshType %d.\n", this->MeshType);
        } break;
    }
}

SpecialMesh *SpecialMesh_new(const char *className, Instance *parent)
{
    SpecialMesh *newInst = FileMesh_new("SpecialMesh", parent);

    newInst->filemesh.datamodelmesh.instance.DataCost = sizeof(SpecialMesh);
    newInst = realloc(newInst, sizeof(SpecialMesh));

    newInst->MeshType = MeshType_Head;
    newInst->filemesh.datamodelmesh.drawFunc = draw_specialmesh;

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

void serialize_SpecialMesh(SpecialMesh *specialmesh, SerializeInstance *inst)
{
    serialize_FileMesh(specialmesh, inst);
    
    serialize_atomic(token, specialmesh, MeshType);
}
