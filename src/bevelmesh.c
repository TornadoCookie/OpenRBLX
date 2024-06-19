#include "bevelmesh.h"

BevelMesh *BevelMesh_new(const char *className, Instance *parent)
{
    BevelMesh *newInst = DataModelMesh_new(className, parent);

    newInst->datamodelmesh.instance.DataCost = sizeof(BevelMesh);
    newInst = realloc(newInst, sizeof(BevelMesh));

    return newInst;
}

void serialize_BevelMesh(BevelMesh *bevelmesh, SerializeInstance *inst)
{
    serialize_DataModelMesh(bevelmesh, inst);

    serialize_atomic(float, bevelmesh, Bevel);
    _serialize_atomic(inst, (Serialization){Serialize_float, "Bevel Roundness", &bevelmesh->BevelRoundness});
    serialize_atomic(float, bevelmesh, Bulge);
}
