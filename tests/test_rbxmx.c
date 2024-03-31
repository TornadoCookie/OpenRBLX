#include <raylib.h>
#include "datamodel.h"
#include "filetypes.h"
#include <malloc.h>

#define DEBUG_IMPL
#include "debug.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("usage: test_rbxmx <mdl.rbxmx>\n");
        return 0;
    }
    InitWindow(640, 480, "rbxmx test");
    DataModel *datamodel = DataModel_new();
    int mdlCount;
    Instance **mdls = LoadModelRBXMX(argv[1], &mdlCount);

    if (!mdls)
    {
        perror("file error");
        return 0;
    }

    for (int i = 0; i < mdlCount; i++)
    {
        Instance_SetParent(mdls[i], datamodel->Workspace);
    }
    free(mdls);

    SetTargetFPS(60);
    DisableCursor();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        DataModel_Draw(datamodel);
        EndDrawing();
    }
    
    DataModel_Shutdown(datamodel);

    return 0;
}
