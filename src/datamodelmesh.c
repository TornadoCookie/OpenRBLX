#include "datamodelmesh.h"
#include "basepart.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(datamodelmesh)

void datamodelmesh_draw(DataModelMesh *this, BasePart *parent)
{
    FIXME("classname %s has no draw func.\n", this->instance.ClassName);
}

DataModelMesh *DataModelMesh_new(const char *className, Instance *parent)
{
    DataModelMesh *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(DataModelMesh);
    newInst = realloc(newInst, sizeof(DataModelMesh));
    
    newInst->drawFunc = datamodelmesh_draw;

    return newInst;
}

void DataModelMesh_Draw(DataModelMesh *this, BasePart *parent)
{
    this->drawFunc(this, parent);
}

void serialize_DataModelMesh(DataModelMesh *datamodelmesh, SerializeInstance *inst)
{
    serialize_Instance(datamodelmesh, inst);

    serialize_atomic(Vector3, datamodelmesh, Offset);
    serialize_atomic(Vector3, datamodelmesh, Scale);
    serialize_atomic(Vector3, datamodelmesh, VertexColor);
    serialize_atomic(token, datamodelmesh, LODX);
    serialize_atomic(token, datamodelmesh, LODY);
}
