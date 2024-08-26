#include "meshcontentprovider.h"
#include <raylib.h>
#include <stdlib.h>
#include "debug.h"
#include <rlgl.h>
#include <string.h>
#include "httpservice.h"
#include "datamodel.h"
#include <mini_gzip.h>

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

Mesh LoadMeshFromRobloxFormat(const char *data, int dataSize)
{
    float version;
    printf("%p: %s\n", data, data);
    sscanf(data, "version %f", &version);
    data = strchr(data, '\n')+1;

    if (version != 1.0f)
    {
        FIXME("Unable to load mesh format version %f.\n", version);
        return (Mesh){ 0 };
    }

    Mesh mesh = { 0 };

    sscanf(data, "%d", &mesh.triangleCount);
    data = strchr(data, '\n')+1;

    mesh.vertexCount = mesh.triangleCount * 3;

    mesh.vertices = malloc(sizeof(float) * mesh.vertexCount * 3);
    mesh.texcoords = malloc(sizeof(float) * mesh.vertexCount * 2);
    mesh.normals = malloc(sizeof(float) * mesh.vertexCount * 3);

    printf("Version: %f\n", version);
    printf("Triangle Count: %d\n", mesh.triangleCount);

    char *theRest = data;

    for (int i = 0; i < mesh.vertexCount; i++)
    {
        sscanf(theRest, "[%f,%f,%f]", &(mesh.vertices[i*3]), &(mesh.vertices[i*3+1]), &(mesh.vertices[i*3+2]));
        printf("Vertex: [%f, %f, %f]\n", mesh.vertices[i*3], mesh.vertices[i*3+1], mesh.vertices[i*3+2]);
        mesh.vertices[i*3] /= 2;
        mesh.vertices[i*3+1] /= 2;
        mesh.vertices[i*3+2] /= 2;
        theRest = strchr(theRest, ']') + 1;
        sscanf(theRest, "[%f,%f,%f]", &(mesh.normals[i*3]), &(mesh.normals[i*3+1]), &(mesh.normals[i*3+2]));
        theRest = strchr(theRest, ']') + 1;
        float dummyThirdTexCoord;
        sscanf(theRest, "[%f,%f,%f]", &(mesh.texcoords[i*2]), &(mesh.texcoords[i*2+1]), &dummyThirdTexCoord);
        mesh.texcoords[i*2+1] = 1 - mesh.texcoords[i*2+1];
        theRest = strchr(theRest, ']') + 1;
    }


    UploadMesh(&mesh, false);

    return mesh;
}

Mesh MeshContentProvider_GetFileMesh(MeshContentProvider *this, const char *content)
{
    int assetid;

    sscanf(content, "http://www.roblox.com/asset/?id=%d", &assetid);

    printf("Get AssetId %d\n", assetid);
    const char *url = TextFormat("https://assetdelivery.roblox.com/v2/assetId/%d", assetid);
    printf("Our thing is: %s\n", url);

    HttpService *httpService = ServiceProvider_GetService(GetDataModel(), "HttpService");
    const char *jsonundecoded = HttpService_GetAsync(httpService, url, NULL);
    cJSON *json = HttpService_JSONDecode(httpService, jsonundecoded);

    printf("%s\n", jsonundecoded);

    cJSON *locations = cJSON_GetObjectItem(json, "locations");
    cJSON *location = cJSON_GetArrayItem(locations, 0);
    const char *newUrl = cJSON_GetStringValue(cJSON_GetObjectItem(location, "location"));
    printf("Redirecting to %s.\n", newUrl);

    int dataSize;
    const char *data = HttpService_GetAsync(httpService, newUrl, &dataSize);

    //struct mini_gzip gz;
    //mini_gz_start(&gz, dataCompressed, dataCompressedSize);

    //int uncompressSize;
    //char *data = DecompressData(dataCompressed + 10, dataCompressedSize, &uncompressSize);

    

    //printf("%d\n", mini_gz_unpack(&gz, data, gz.data_len));
    

    Mesh ret = LoadMeshFromRobloxFormat(data, dataSize);
    
    //printf("Unable to retrieve mesh data.\n");

    free(data);

    return ret;
}
