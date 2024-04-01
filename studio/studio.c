#include <raylib.h>
#include "datamodel.h"

#define DEBUG_IMPL
#include "debug.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define MAX_TOOLBAR_BUTTONS 10

typedef struct StudioUIButton {
    void *func;
    const char *name;
} StudioUIButton;

typedef struct StudioUIToolbar {
    const char *name;
    StudioUIButton buttons[MAX_TOOLBAR_BUTTONS];
} StudioUIToolbar;

typedef struct StudioUILayout {
    int toolbarCount;
    StudioUIToolbar toolbars[];
} StudioUILayout;

static const StudioUILayout uiLayout = {
    // layout
    1,
    {
        // toolbar
        "File",
        {
            // buttons
            {NULL, "New"},
            {NULL, "Open"},
            {NULL, "Save"},
            {NULL, "Save As"},
        }
    }
};

typedef struct StudioUIState {
    char *status;
    int topbarActive;
    char *topBarText;
} StudioUIState;

static StudioUIState uiState;

static void load_studio_ui()
{
    int totalSize = 0;
    for (int i = 0; i < uiLayout.toolbarCount; i++)
    {
        int oldTotalSize = totalSize;
        totalSize += strlen(uiLayout.toolbars[i].name) + 1;
        uiState.topBarText = realloc(uiState.topBarText, totalSize + 1);
        memcpy(uiState.topBarText + oldTotalSize, uiLayout.toolbars[i].name, strlen(uiLayout.toolbars[i].name));
        if (i != uiLayout.toolbarCount - 1) uiState.topBarText[totalSize - 1] = ';';
        uiState.topBarText[totalSize] = 0;
    }
}

static void draw_studio_ui()
{
    // status bar
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    GuiStatusBar((Rectangle){ 0, (float)GetScreenHeight() - 20, (float)GetScreenWidth(), 20 }, uiState.status);
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

    // top bar
    GuiComboBox((Rectangle){0, 0, 60, 20}, uiState.topBarText, &uiState.topbarActive);
    for (int i = 0; i < MAX_TOOLBAR_BUTTONS; i++)
    {
        if (!uiLayout.toolbars[uiState.topbarActive].buttons[i].name) continue;
        GuiButton((Rectangle){60 + i * 50, 0, 50, 20}, uiLayout.toolbars[uiState.topbarActive].buttons[i].name);
    }
}

int main()
{
    InitWindow(640, 480, "OpenRBLX Studio");
    SetTargetFPS(60);
    DataModel *datamodel = DataModel_new();
    SetExitKey(KEY_F1);

    load_studio_ui();

    uiState.status = "Ready.";

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
