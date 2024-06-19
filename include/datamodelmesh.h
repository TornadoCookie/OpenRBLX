#ifndef _DATAMODELMESH_
#define _DATAMODELMESH_

#include "instance.h"
#include "basepart.h"

typedef struct DataModelMesh DataModelMesh;

typedef enum {
    LevelOfDetailSetting_Low,
    LevelOfDetailSetting_Medium,
    LevelOfDetailSetting_High,
} LevelOfDetailSetting;

typedef struct DataModelMesh {
    Instance instance;

    Vector3 Offset;
    Vector3 Scale;
    Vector3 VertexColor;

    LevelOfDetailSetting LODX;
    LevelOfDetailSetting LODY;

    void (*drawFunc) (DataModelMesh*, BasePart*);
} DataModelMesh;

DataModelMesh *DataModelMesh_new(const char *className, Instance *parent);

void DataModelMesh_Draw(DataModelMesh *this, BasePart *parent);

void serialize_DataModelMesh(DataModelMesh *datamodelmesh, SerializeInstance *inst);

#endif
