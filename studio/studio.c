#include <raylib.h>
#include "datamodel.h"

#define DEBUG_IMPL
#include "debug.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

static char *status = "Loading...";

static void draw_studio_ui()
{
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    GuiStatusBar((Rectangle){ 0, (float)GetScreenHeight() - 20, (float)GetScreenWidth(), 20 }, status);
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
}

int main()
{
    InitWindow(640, 480, "OpenRBLX Studio");
    SetTargetFPS(60);
    DataModel *datamodel = DataModel_new();
    SetExitKey(KEY_F1);

    status = "Ready.";

    while (!WindowShouldClose())
    {
        BeginDrawing();
        DataModel_Draw(datamodel);
        draw_studio_ui();
        if (IsKeyPressed(KEY_ESCAPE))
        {
            if (GuiIsLocked())
            {
                GuiUnlock();
                EnableCursor();
            }
            else
            {
                GuiLock();
                DisableCursor();
            }
        }
        EndDrawing();
    }

    DataModel_Shutdown(datamodel);
    return 0;
}
