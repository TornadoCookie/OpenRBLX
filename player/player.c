#include <stdio.h>
#include "datamodel.h"
#include <raylib.h>
#include <string.h>
#include "httpservice.h"
#include "filetypes.h"
#include "players.h"
#include "runservice.h"
#include "cacheablecontentprovider.h"
#include "localscript.h"

#define DEBUG_IMPL
#include "debug.h"

void usage(void)
{
    printf("Usage: OpenRblx [options] <file>\n");
    printf("Where <file> can be:\n");
    printf("\t- A game ID (will always be interpreted as a game ID)\n");
    printf("\t- A path to a .rbxl or .rbxlx file\n");
    printf("\t- A path to a .rbxm or .rbxmx file\n");
}

void AttemptLoadFile(DataModel *game, const char *file)
{
    FILE *f = fopen(file, "rb");
    char sig[8];
    bool isModel = !strncmp(GetFileExtension(file), ".rbxm", 5);
    bool isBinary = false;

    fread(sig, 1, 8, f);

    fclose(f);

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

int main(int argc, char **argv)
{
    char *gameToLoad = NULL;

    for (int i = 1; i < argc; i++)
    {
        if (*(argv[i]) == '-')
        {
            // TODO
        }
        else if (gameToLoad)
        {
            printf("Expected one game to load.\n");
            usage();
            return 1;
        }
        else
        {
            gameToLoad = argv[i];
        }
    }

    if (!gameToLoad)
    {
        printf("Expected one game to load.\n");
        usage();
        return 1;
    }

    InitWindow(1280, 720, "OpenRblx");
    DataModel *game = DataModel_new();

    if (FileExists(gameToLoad))
    {
        AttemptLoadFile(game, gameToLoad);
    }
    else if (atol(gameToLoad))
    {
        int dataSize = 0;
        // big nono using NULL here but CacheableContentProvider doesn't use itself
        const char *data = CacheableContentProvider_LoadAssetDelivery(NULL, atol(gameToLoad), &dataSize);
        const char *gamePath = TextFormat("cache/game_%s.rbxl", gameToLoad);

        SaveFileData(gamePath, data, dataSize);
        AttemptLoadFile(game, gamePath);
    }

    SetTargetFPS(60);
    DisableCursor();

    // CoreScripts loader
    LocalScript *scr = LocalScript_new("LocalScript", Instance_FindFirstChildOfClass(ServiceProvider_GetService(game, "StarterPlayer"), "StarterPlayerScripts"));
    scr->script.Source = "require(game.PlayerScripts.StarterPlayerScripts.PlayerScriptsLoader)";
    scr->script.sourceLength = strlen(scr->script.Source);

    Players *players = ServiceProvider_GetService(game, "Players");
    Players_CreateLocalPlayer(players);
    Player *localPlayer = players->LocalPlayer;
    Player_LoadCharacter(localPlayer); 

    RunService *runService = ServiceProvider_GetService(game, "RunService");
    RunService_Run(runService);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        DataModel_Draw(game);
        EndDrawing();
    }

    DataModel_Shutdown(game);

    return 0;
}

