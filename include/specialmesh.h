#ifndef _SPECIALMESH_
#define _SPECIALMESH_

#include "filemesh.h"

typedef enum {
    MeshType_Head,
    MeshType_Torso,
    MeshType_Wedge,
    MeshType_Sphere,
    MeshType_Cylinder,
    MeshType_FileMesh,
    MeshType_Brick,
    MeshType_Prism,
    MeshType_Pyramid,
    MeshType_ParallelRamp,
    MeshType_RightAngleRamp,
    MeshType_CornerWedge,
} MeshType;

typedef struct SpecialMesh {
    FileMesh filemesh;

    MeshType MeshType;
} SpecialMesh;

SpecialMesh *SpecialMesh_new(const char *className, Instance *parent);

void serialize_SpecialMesh(SpecialMesh *specialmesh, SerializeInstance *inst);

#endif
