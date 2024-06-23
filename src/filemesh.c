#include "filemesh.h"
#include "meshcontentprovider.h"
#include "datamodel.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(filemesh)

static Color rl_from_color3(Color3 col, float transparency)
{
    return (Color){col.R * 255, col.G * 255, col.B * 255, (1.0f-transparency) * 255};
}

void FileMesh_Draw(FileMesh *this, BasePart *part)
{
    if (!this->meshLoaded)
    {
        MeshContentProvider *mcp = ServiceProvider_GetService(GetDataModel(), "MeshContentProvider");
        this->mesh = MeshContentProvider_GetFileMesh(mcp, this->MeshId);
        this->meshLoaded = true;
    }

    this->material.maps[MATERIAL_MAP_DIFFUSE].color = rl_from_color3(part->Color, part->Transparency);

    if (this->mesh.vertexCount) DrawMesh(this->mesh, this->material, MatrixTranslate(part->Position.x, part->Position.y, part->Position.z));
}

FileMesh *FileMesh_new(const char *className, Instance *parent)
{
    FileMesh *newInst = DataModelMesh_new(className, parent);

    newInst->datamodelmesh.instance.DataCost = sizeof(FileMesh);
    newInst = realloc(newInst, sizeof(FileMesh));

    newInst->datamodelmesh.drawFunc = FileMesh_Draw;
    newInst->meshLoaded = false;
    newInst->material = LoadMaterialDefault();

    return newInst;
}

void serialize_FileMesh(FileMesh *filemesh, SerializeInstance *inst)
{
    serialize_DataModelMesh(filemesh, inst);

    serialize_atomic(string, filemesh, MeshId);
    serialize_atomic(string, filemesh, TextureId);
}
