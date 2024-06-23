#include "meshcontentprovider.h"
#include <raylib.h>
#include <stdlib.h>
#include "debug.h"
#include <rlgl.h>
#include <string.h>
#include "httpservice.h"
#include "datamodel.h"

DEFAULT_DEBUG_CHANNEL(meshcontentprovider)

static Mesh GenMeshWedge()
{
    return LoadModel("staticdata/wedge.obj").meshes[0];
}

MeshContentProvider *MeshContentProvider_new(const char *className, Instance *parent)
{
    MeshContentProvider *newInst = CacheableContentProvider_new("MeshContentProvider", parent);

    newInst->cacheablecontentprovider.instance.DataCost = sizeof(MeshContentProvider);
    newInst = realloc(newInst, sizeof(MeshContentProvider));

    // We use 0.5f as the radius for curved shapes because BasePart.Size is in diameter.
    newInst->ballMesh = GenMeshSphere(0.5f, 12, 24);
    newInst->blockMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    newInst->cylinderMesh = GenMeshCylinder(0.5f, 1.0f, 24);
    newInst->wedgeMesh = GenMeshWedge();

    newInst->studs = LoadTexture("staticdata/studs.png");

    newInst->cacheablecontentprovider.instance.Name = "MeshContentProvider";

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

Mesh MeshContentProvider_GetPartMesh(MeshContentProvider *this, Shape shape)
{
    switch (shape)
    {
        case Shape_Ball: return this->ballMesh;
        case Shape_Block: return this->blockMesh;
        case Shape_Cylinder: return this->cylinderMesh;
        default: FIXME("shape %d not implemented.\n", shape); break;
    }
}

/*
Mesh LoadMeshFromRobloxFormat(unsigned char *data, int dataSize)
{
    float version;
    printf("%s\n", buf);
    sscanf(data, "version %f", &version);

    if (version != 1.0f)
    {
        FIXME("Unable to load mesh format version %f.\n", version);
        return (Mesh){ 0 };
    }

    Mesh mesh = { 0 };

    fgets(buf, 255, f);
    sscanf(buf, "%d", &mesh.triangleCount);

    mesh.vertexCount = mesh.triangleCount * 3;

    mesh.vertices = malloc(sizeof(float) * mesh.vertexCount * 3);
    mesh.texcoords = malloc(sizeof(float) * mesh.vertexCount * 2);
    mesh.normals = malloc(sizeof(float) * mesh.vertexCount * 3);

    long offset = ftell(f);
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, offset, SEEK_SET);

    char *theRest = malloc(fsize - offset + 1);
    fgets(theRest, fsize - offset, f);
    theRest[fsize - offset] = 0;

    for (int i = 0; i < mesh.vertexCount; i++)
    {
        sscanf(theRest, "[%f,%f,%f]", &(mesh.vertices[i*3]), &(mesh.vertices[i*3+1]), &(mesh.vertices[i*3+2]));
        mesh.vertices[i*3] /= 2;
        mesh.vertices[i*3+1] /= 2;
        mesh.vertices[i*3+2] /= 2;
        theRest = strchr(theRest, ']') + 1;
        sscanf(theRest, "[%f,%f,%f]", &(mesh.normals[i*3]), &(mesh.normals[i*3+1]), &(mesh.normals[i*3+2]));
        theRest = strchr(theRest, ']') + 1;
        sscanf(theRest, "[%f,%f,0]", &(mesh.texcoords[i*2]), &(mesh.texcoords[i*2+1]));
        mesh.texcoords[i*2+1] = 1 - mesh.texcoords[i*2+1];
    }

    return mesh;
}*/

Mesh MeshContentProvider_GetFileMesh(MeshContentProvider *this, const char *content)
{
    int assetid;

    sscanf(content, "http://www.roblox.com/asset/?id=%d", &assetid);

    printf("Get AssetId %d\n", assetid);
    const char *url = TextFormat("https://assetdelivery.roblox.com/v2/assetId/%d", assetid);
    printf("Our thing is: %s\n", url);

    HttpService *httpService = ServiceProvider_GetService(GetDataModel(), "HttpService");
    const char *data = HttpService_GetAsync(httpService, url);

    //Mesh ret = LoadMeshFromRobloxFormat(data, strlen(data));
    
    printf("Unable to retrieve mesh data.\n");

    //free(data);

    return (Mesh){0};
}
