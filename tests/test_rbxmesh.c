#include "datamodel.h"
#include "meshcontentprovider.h"
#include "meshpart.h"
#include "raylib.h"

#define DEBUG_IMPL
#include "debug.h"

int main(int argc, char **argv)
{
    InitWindow(640, 480, "rbxmesh test");
    DataModel *game = DataModel_new();

    MeshContentProvider *mcp = ServiceProvider_GetService(game, "MeshContentProvider");

    Mesh mesh = MeshContentProvider_GetFileMesh(mcp, argv[1]);

    Material mat = LoadMaterialDefault();

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        BeginMode3D(game->Workspace->CurrentCamera->camera);
        Camera_Process(game->Workspace->CurrentCamera);
        DrawMesh(mesh, mat, MatrixTranslate(0, 0, 0));
        EndMode3D();
        EndDrawing();
    }

    return 0;
}
