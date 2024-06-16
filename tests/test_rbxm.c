#include <raylib.h>
#include "datamodel.h"
#include "filetypes.h"
#include <malloc.h>

#define DEBUG_IMPL
#include "debug.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("usage: test_rbxm <mdl.rbxm>\n");
        return 0;
    }
    InitWindow(640, 480, "rbxm test");
    DataModel *datamodel = DataModel_new();
    int mdlCount;
    Instance **mdls = LoadModelRBXM(argv[1], &mdlCount);

    if (!mdls)
    {
        perror("file error");
        return 0;
    }

    for (int i = 0; i < mdlCount; i++)
    {
        if (!mdls[i]) continue;
        printf("\nParenting instance to datamodel:\n");
        printf("Pointer %p.\n", mdls[i]);
        printf("Classname %s.\n", mdls[i]->ClassName);
        Instance_SetParent(mdls[i], datamodel->Workspace);
    }
    free(mdls);

    if (argc > 2 && !strcmp(argv[2], "-q")) return 0;

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
