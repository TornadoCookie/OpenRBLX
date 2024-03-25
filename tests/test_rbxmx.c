#include <raylib.h>
#include "datamodel.h"
#include "filetypes.h"

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
    Instance *mdl = LoadModelRBXMX(argv[1]);

    Instance_SetParent(mdl, datamodel->Workspace);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        DataModel_Draw(datamodel);
        EndDrawing();
    }
    
    DataModel_Shutdown(datamodel);
    return 0;
}
