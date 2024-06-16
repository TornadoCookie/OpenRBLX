#ifndef _CYLINDERMESH_
#define _CYLINDERMESH_

#include "bevelmesh.h"

typedef struct CylinderMesh {
    BevelMesh bevelmesh;
} CylinderMesh;

CylinderMesh *CylinderMesh_new(const char *className, Instance *parent);

void serialize_CylinderMesh(CylinderMesh *cylindermesh, SerializeInstance *inst);

#endif
