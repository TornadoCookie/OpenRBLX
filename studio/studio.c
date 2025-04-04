#include <raylib.h>
#include "datamodel.h"
#include "filetypes.h"
#include "scriptruntime.h"
#include "folder.h"
#include "runservice.h"
#include "plugin.h"

#include "studio/corepackages.h"

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

static Vector2 draw_explorer_list(Instance *inst, Vector2 position, Vector2 scroll)
{
    int childCount;
    Instance **children = Instance_GetChildren(inst, &childCount);
    for (int i = 0; i < childCount; i++)
    {
        GuiLabel((Rectangle){position.x + 20, position.y + (i+1)*20 + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + scroll.y, 100, 10}, children[i]->Name);
        position = draw_explorer_list(children[i], (Vector2){position.x + 20, position.y + (i+1)*20}, scroll);
    }

    return (Vector2){position.x - 20, position.y + childCount*20};
}

static void draw_explorer_window(Vector2 position, Vector2 scroll)
{
    draw_explorer_list(GetDataModel(), position, scroll);
}

static void draw_studio_ui()
{
    // status bar
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    GuiStatusBar((Rectangle){ 0, (float)GetScreenHeight() - 20, (float)GetScreenWidth(), 20 }, uiState.status);
    //GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

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

void AttemptLoadFile(DataModel *game, const char *file)
{
    FILE *f = fopen(file, "rb");
    char sig[8];
    bool isModel = !strncmp(GetFileExtension(file), ".rbxm", 5);
    bool isBinary = false;

    fread(sig, 1, 8, f);

    fclose(f);

    if (TextIsEqual(GetFileExtension(file), ".pack"))
    {
        int shaderCount = 0;
        //Shader *shdrs = LoadShadersRBXS(file, &shaderCount);
        exit(EXIT_SUCCESS);
    }

    if (!strncmp(sig, "<roblox!", 8))
    {
        isBinary = true;
    }

    if (isModel)
    {
        int mdlCount;
        Instance **mdl;

        if (isBinary)
        {
            mdl = LoadModelRBXM(file, &mdlCount);
        }
        else
        {
            mdl = LoadModelRBXMX(file, &mdlCount);
        }

        for (int i = 0; i < mdlCount; i++)
        {
            Instance_SetParent(mdl[i], game->Workspace);
        }
    }
    else
    {
        if (isBinary)
        {
            int mdlCount;
            Instance **mdl;

            mdl = LoadModelRBXM(file, &mdlCount);

            for (int i = 0; i < mdlCount; i++)
            {
                Instance_SetParent(mdl[i], game);
            }
        }
        else
        {
            LoadPlaceRBXLX(file);
        }
    }
}

static void detail_tree(int level, Instance *inst)
{
    for (int i = 0; i < level; i++) printf("\t");
    printf("%s %s\n", inst->ClassName, inst->Name);

    for (int i = 0; i < inst->childCount; i++)
    {
        detail_tree(level+1, inst->children[i]);
    }
}

static void load_plugins()
{
    int mdlCount = 0;
    Instance **explorerPlugin_s = LoadModelRBXM("studioRoot/BuiltInStandalonePlugins/Optimized_Embedded_Signature/StartPage.rbxm", &mdlCount);

    Instance_SetParent(explorerPlugin_s[0], explorerPlugin_s[1]);

    detail_tree(0, explorerPlugin_s[1]);

    Plugin *plugin = Plugin_new("Plugin", NULL);
    Instance_SetParent(explorerPlugin_s[1], plugin);

    ScriptRuntime *scrt = ScriptRuntime_new("ScriptRuntime", NULL);
    ScriptRuntime_RunPluginScript(scrt, explorerPlugin_s[0], plugin);

    pthread_exit(NULL);
}

static void load_corescripts(DataModel *game)
{
    CorePackages_new("CorePackages", game);
}

bool RunService_IsStudio(RunService *this)
{
    return true;
}

int main()
{
    InitWindow(1280, 720, "OpenRBLX Studio");
    SetTargetFPS(60);
    DataModel *datamodel = DataModel_new();
    SetExitKey(KEY_F1);

    load_studio_ui();

    load_plugins();

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
                AttemptLoadFile(datamodel, x);
            }
        }
    }

    DataModel_Shutdown(datamodel);
    return 0;
}
