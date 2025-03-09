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

#define GUI_FLOATING_WINDOW_IMPLEMENTATION
#include "gui_floating_window.h"

typedef struct StudioUIState {
    char *status;
    int topbarActive;
    char *topBarText;
    GuiWindowFileDialogState windowFileDialogState;
    bool guiFocus;
    GuiFloatingWindowState explorerWindowState;
    int explorerWindowActive;
    Instance *explorerSelection;
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

#define LOAD_STRING_INTO_SEMICOLON_SEPARATED(count, way, dest) \
    {int totalSize = dest ? strlen(dest) : 0; \
    for (int i = 0; i < count; i++) \
    { \
        int oldTotalSize = totalSize; \
        totalSize += strlen(way) + 1; \
        dest = realloc(dest, totalSize + 1); \
        memcpy(dest + oldTotalSize, way, strlen(way)); \
        if (i != count - 1) dest[totalSize - 1] = ';'; \
        dest[totalSize] = 0; \
    }}

static void load_studio_ui()
{
    LOAD_STRING_INTO_SEMICOLON_SEPARATED(uiLayout.toolbarCount, uiLayout.toolbars[i].name, uiState.topBarText)    

    uiState.windowFileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());
    uiState.explorerWindowState.position = (Vector2){0, 40};
    uiState.explorerWindowState.size = (Vector2){200, 300};
    uiState.explorerWindowActive = 0;
    uiState.explorerSelection = GetDataModel();
}

static void draw_explorer_window(Vector2 position, Vector2 scroll)
{
    int childCount;
    Instance **children = Instance_GetChildren(GetDataModel(), &childCount);
    for (int i = 0; i < childCount; i++)
    {
        GuiLabel((Rectangle){position.x, position.y + (i+1)*20 + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, 100, 10}, children[i]->Name);
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
        if (GuiButton((Rectangle){60 + i * 50, 0, 50, 20}, uiLayout.toolbars[uiState.topbarActive].buttons[i].name))
        {
            if (uiLayout.toolbars[uiState.topbarActive].buttons[i].func) uiLayout.toolbars[uiState.topbarActive].buttons[i].func();
        }
    }

    // explorer window
    GuiWindowFloating(&uiState.explorerWindowState, draw_explorer_window, uiState.explorerWindowState.size, "Explorer");
}

int main()
{
    InitWindow(1280, 720, "OpenRBLX Studio");
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
