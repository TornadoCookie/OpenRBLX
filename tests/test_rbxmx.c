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
    InitWindow(640, 480, "OpenRBLX");
    DataModel *datamodel = DataModel_new();
    Model *mdl = LoadModelRBXMX(argv[1]);

    Instance_SetParent(mdl, datamodel);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        DataModel_Draw(datamodel);
        BeginDrawing();
    }
    
    DataModel_Shutdown(datamodel);
    return 0;
}
