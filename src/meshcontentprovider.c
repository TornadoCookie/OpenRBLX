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

Mesh LoadMeshRBXV1(const char *data, int dataSize)
{
    Mesh mesh = { 0 };

    sscanf(data, "%d", &mesh.triangleCount);
    data = strchr(data, '\n')+1;

    mesh.vertexCount = mesh.triangleCount * 3;

    mesh.vertices = malloc(sizeof(float) * mesh.vertexCount * 3);
    mesh.texcoords = malloc(sizeof(float) * mesh.vertexCount * 2);
    mesh.normals = malloc(sizeof(float) * mesh.vertexCount * 3);

    printf("Triangle Count: %d\n", mesh.triangleCount);

    char *theRest = data;

    for (int i = 0; i < mesh.vertexCount; i++)
    {
        sscanf(theRest, "[%f,%f,%f]", &(mesh.vertices[i*3]), &(mesh.vertices[i*3+1]), &(mesh.vertices[i*3+2]));
        //printf("Vertex: [%f, %f, %f]\n", mesh.vertices[i*3], mesh.vertices[i*3+1], mesh.vertices[i*3+2]);
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

typedef struct Vertex2 {
    float px, py, pz;
    float nx, ny, nz;
    float tu, tv;

    int8_t tx, ty, tz, ts;
    uint8_t  r, g, b, a;
} Vertex2;

typedef struct Face2 {
    uint32_t a;
    uint32_t b;
    uint32_t c;
} Face2;

typedef struct MeshHeader2 {
    uint16_t size;
    uint8_t vertexSize;
    uint8_t faceSize;

    uint32_t vertexCount;
    uint32_t faceCount;
} MeshHeader2;

Mesh LoadMeshRBXV2(const char *data, int dataSize)
{
    Mesh mesh = { 0 };

    MeshHeader2 header = *(MeshHeader2*)data;

    printf("Header Size: %d\n", header.size);
    printf("Vertex Size: %d\n", header.vertexSize);
    printf("Face Size: %d\n", header.faceSize);
    printf("Vertex Count: %d\n", header.vertexCount);
    printf("Face Count: %d\n", header.faceCount);

    mesh.triangleCount = header.faceCount;
    mesh.vertexCount = mesh.triangleCount * 3;
    mesh.vertices = malloc(sizeof(float) * mesh.vertexCount * 3);
    mesh.texcoords = malloc(sizeof(float) * mesh.vertexCount * 2);
    mesh.normals = malloc(sizeof(float) * mesh.vertexCount * 3);
    if (header.vertexSize == 40) mesh.colors = malloc(mesh.vertexCount * 4);

    data += sizeof(MeshHeader2);

    void *vertices = data;
    data += header.vertexSize * header.vertexCount;

    Face2 *faces = data;
    data += header.faceSize * header.faceCount;

    for (int i = 0; i < header.faceCount; i++)
    {
        Face2 face = faces[i];

        uint32_t *faceArr = &face;

        for (int j = 0; j < 3; j++)
        {
            Vertex2 v = *(Vertex2*)(vertices+header.vertexSize*(i*9+j*3)); //EWWWWWWW
            //printf("%d\n", i*9+j*3);

            mesh.vertices[i*9+j*3+0] = v.px;
            mesh.vertices[i*9+j*3+1] = v.py;
            mesh.vertices[i*9+j*3+2] = v.pz;

            mesh.normals[i*9+j*3+0] = v.nx;
            mesh.normals[i*9+j*3+1] = v.ny;
            mesh.normals[i*9+j*3+2] = v.nz;

            mesh.texcoords[i*6+j*2+0] = v.tu;
            mesh.texcoords[i*6+j*2+1] = v.tv;

            if (header.vertexSize == 40)
            {
                mesh.colors[i*12+j*4+0] = v.r;
                mesh.colors[i*12+j*4+1] = v.g;
                mesh.colors[i*12+j*4+2] = v.b;
                mesh.colors[i*12+j*4+3] = v.a;
            }
        }
    }

    UploadMesh(&mesh, false);

    return mesh;
}

typedef struct MeshHeader4 {
    uint16_t size;
    uint16_t lodType;
    uint32_t vertexCount;
    uint32_t faceCount;
    uint16_t lodCount;
    uint16_t boneCount;
    uint32_t boneNameBufferSize;
    uint16_t subsetCount;
    uint8_t hqLodCount;
    uint8_t unused;
} MeshHeader4;

Mesh LoadMeshRBXV4(const char *data, int dataSize)
{
    Mesh mesh = { 0 };

    MeshHeader4 header = *(MeshHeader4*)data;

    printf("Header Size: %d\n", header.size);
    printf("LOD Type: %d\n", header.lodType);
    printf("Vertex Count: %d\n", header.vertexCount);
    printf("Face Count: %d\n", header.faceCount);
    printf("LOD Count: %d\n", header.lodCount);
    printf("Bone Count: %d\n", header.boneCount);
    printf("Bone name buffer size: %d\n", header.boneNameBufferSize);
    printf("Subset count: %d\n", header.subsetCount);
    printf("HQ LOD count: %d\n", header.hqLodCount);

    mesh.triangleCount = header.faceCount;
    mesh.vertexCount = mesh.triangleCount * 3;
    mesh.vertices = malloc(sizeof(float) * mesh.vertexCount * 3);
    mesh.texcoords = malloc(sizeof(float) * mesh.vertexCount * 2);
    mesh.normals = malloc(sizeof(float) * mesh.vertexCount * 3);
    mesh.colors = malloc(mesh.vertexCount * 4);

    data += sizeof(MeshHeader4);

    Vertex2 *vertices = data;
    data += sizeof(Vertex2) * header.vertexCount;

    Face2 *faces = data;
    data += sizeof(Face2) * header.faceCount;

    uint32_t *lods = data;
    data += sizeof(uint32_t) * header.lodCount;

    // Load LOD 0
    for (int i = lods[0]; i < lods[1]; i++)
    {
        Face2 face = faces[i];

        uint32_t *faceArr = &face;

        for (int j = 0; j < 3; j++)
        {
            Vertex2 v = vertices[faceArr[j]];
            //printf("%d\n", i*9+j*3);

            mesh.vertices[i*9+j*3+0] = v.px;
            mesh.vertices[i*9+j*3+1] = v.py;
            mesh.vertices[i*9+j*3+2] = v.pz;


            mesh.normals[i*9+j*3+0] = v.nx;
            mesh.normals[i*9+j*3+1] = v.ny;
            mesh.normals[i*9+j*3+2] = v.nz;

            mesh.texcoords[i*6+j*2+0] = v.tu;
            mesh.texcoords[i*6+j*2+1] = v.tv;

            mesh.colors[i*12+j*4+0] = v.r;
            mesh.colors[i*12+j*4+1] = v.g;
            mesh.colors[i*12+j*4+2] = v.b;
            mesh.colors[i*12+j*4+3] = v.a;
        }
    }

    UploadMesh(&mesh, false);

    // TODO: Read bone / animation data
    if (header.boneCount) FIXME("TODO read animation data (boneCOunt %d)\n", header.boneCount);

    return mesh;
}

Mesh LoadMeshFromRobloxFormat(const char *data, int dataSize)
{
    float version;
    printf("%p: %s\n", data, data);
    sscanf(data, "version %f", &version);
    data = strchr(data, '\n')+1;

    printf("Version: %f\n", version);

    if (version == 1.0f)
    {
        return LoadMeshRBXV1(data, dataSize);
    }
    else if (version == 2.0f)
    {
        return LoadMeshRBXV2(data, dataSize);
    }
    else if (version == 4.0f)
    {
        return LoadMeshRBXV4(data, dataSize);
    }
    else
    {
        FIXME("Unable to load mesh format version %f.\n", version);
        return (Mesh){ 0 };
    }

    
}

Mesh MeshContentProvider_GetFileMesh(MeshContentProvider *this, const char *content)
{
    long assetid = 0;

    printf("GetFileMesh %s\n", content);
    if (!strncmp(content, "http://www.roblox.com/asset", 27))
    {
        sscanf(content, "http://www.roblox.com/asset/?id=%ld", &assetid);   
    }
    else if (!strncmp(content, "rbxassetid://", 13))
    {
        sscanf(content, "rbxassetid://%ld", &assetid);
    }
    else
    {
        printf("Don't know how to handle this\n");
        return (Mesh){0};
    }

    printf("Get AssetId %ld\n", assetid);

    if (FileExists(TextFormat("cache/%ld.obj", assetid)))
    {
        printf("Using cached\n");
        return LoadModel(TextFormat("cache/%ld.obj", assetid)).meshes[0];
    }

    const char *url = TextFormat("https://assetdelivery.roblox.com/v2/assetId/%ld", assetid);
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

    ExportMesh(ret, TextFormat("cache/%ld.obj", assetid));
    
    //printf("Unable to retrieve mesh data.\n");

    free(data);

    return ret;
}
