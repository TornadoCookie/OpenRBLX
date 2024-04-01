#include <raylib.h>
#include "datamodel.h"
#include "filetypes.h"

#define DEBUG_IMPL
#include "debug.h"

DEFAULT_DEBUG_CHANNEL(studio)

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#undef RAYGUI_IMPLEMENTATION
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_window_file_dialog.h"

typedef struct StudioUIState {
    char *status;
    int topbarActive;
    char *topBarText;
    GuiWindowFileDialogState windowFileDialogState;
    bool guiFocus;
} StudioUIState;

static StudioUIState uiState;

#define MAX_TOOLBAR_BUTTONS 10

typedef struct StudioUIButton {
    void (*func) (void);
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

static void open()
{
    uiState.windowFileDialogState.windowActive = true;
    uiState.windowFileDialogState.saveFileMode = false;
}

static void saveas()
{
    uiState.windowFileDialogState.windowActive = true;
    uiState.windowFileDialogState.saveFileMode = true;
}

static const StudioUILayout uiLayout = {
    // layout
    1,
    {
        // toolbar
        "File",
        {
            // buttons
            {open, "Open"},
            {saveas, "Save As"},
        }
    }
};

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

    uiState.windowFileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());
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
        if (GuiButton((Rectangle){60 + i * 50, 0, 50, 20}, uiLayout.toolbars[uiState.topbarActive].buttons[i].name))
        {
            if (uiLayout.toolbars[uiState.topbarActive].buttons[i].func) uiLayout.toolbars[uiState.topbarActive].buttons[i].func();
        }
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

        if (uiState.windowFileDialogState.windowActive) GuiLock();
        draw_studio_ui();
        if (IsKeyPressed(KEY_ESCAPE))
        {
            if (uiState.guiFocus)
            {
                GuiUnlock();
                EnableCursor();
                uiState.guiFocus = false;
            }
            else
            {
                GuiLock();
                DisableCursor();
                uiState.guiFocus = true;
            }
        }

        GuiUnlock();

        GuiWindowFileDialog(&uiState.windowFileDialogState);
        EndDrawing();

        if (uiState.windowFileDialogState.SelectFilePressed)
        {
            uiState.windowFileDialogState.SelectFilePressed = false;
            if (uiState.windowFileDialogState.saveFileMode)
            {
                FIXME("save files not implemented: %s.\n", uiState.windowFileDialogState.fileNameText);
            }
            else
            {
                const char *x = TextFormat("%s/%s", uiState.windowFileDialogState.dirPathText, uiState.windowFileDialogState.fileNameText);
                if (IsFileExtension(uiState.windowFileDialogState.fileNameText, ".rbxmx"))
                {
                    int objCount;
                    Instance **objs = LoadModelRBXMX(x, &objCount);

                    for (int i = 0; i < objCount; i++)
                    {
                        if (!objs[i]) continue;
                        Instance_SetParent(objs[i], datamodel->Workspace);
                    }
                }
                else if (IsFileExtension(uiState.windowFileDialogState.fileNameText, ".rbxmx"))
                {
                    LoadPlaceRBXLX(x);
                }
            }
        }
    }

    DataModel_Shutdown(datamodel);
    return 0;
}
