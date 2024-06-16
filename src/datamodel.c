#include "camera.h"
#include "datamodel.h"
#include <stddef.h>
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meshcontentprovider.h"
#include "rlgl.h"
#include "lighting.h"

DEFAULT_DEBUG_CHANNEL(datamodel)

static DataModel *game;


static RenderTexture2D rt;

DataModel *DataModel_new(void)
{
    DataModel *newInst = ServiceProvider_new("DataModel", NULL);

    newInst->serviceprovider.instance.DataCost = sizeof(DataModel);
    newInst = realloc(newInst, sizeof(DataModel));

    newInst->AllowedGearTypeChanged = RBXScriptSignal_new();
    newInst->GraphicsQualityChangeRequest = RBXScriptSignal_new();
    newInst->ItemChanged = RBXScriptSignal_new();

    newInst->JobId = "OpenRBLX";

    newInst->Workspace = Workspace_new(newInst);

    if (game)
    {
        TraceLog(LOG_FATAL, "more than one instance of DataModel\n");
    }
    game = newInst;

    MeshContentProvider_new(NULL, newInst);
    Lighting_new(newInst);

    rt = LoadRenderTexture(128, 128);

    return newInst;
}

JobInfo *DataModel_GetJobsInfo(DataModel *this, int *jobsInfoCount)
{
    FIXME("this %p, jobsInfoCount %p stub!\n", this, jobsInfoCount);

    *jobsInfoCount = 0;
    return NULL;
}

bool DataModel_GetRemoteBuildMode(DataModel *this)
{
    FIXME("this %p stub!\n", this);

    return false;
}

const char *DataModel_HttpGetAsync(DataModel *this, const char *url, int httpRequestType)
{
    FIXME("this %p, url \"%s\", httpRequestType %d stub!\n", this, url, httpRequestType);

    return NULL;
}

const char *DataModel_HttpPostAsync(DataModel *this, const char *url, const char *data, const char *contentType, int httpRequestType)
{
    FIXME("this %p, url \"%s\", data \"%s\", contentType \"%s\", httpRequestType %d stub!\n", this, url, data, contentType, httpRequestType);

    return NULL;
}

bool DataModel_IsGearTypeAllowed(DataModel *this, int gearType)
{
    FIXME("this %p, gearType %d stub!\n", this, gearType);

    return true;
}

void DataModel_Load(DataModel *this, const char *url)
{
    FIXME("this %p, url \"%s\" stub!\n", this, url);
}

void DataModel_Shutdown(DataModel *this)
{
    FIXME("this %p semi-stub\n", this);
    RBXScriptSignal_Fire(this->serviceprovider.Close, NULL);
}

static void draw_recursive(Instance *inst)
{
    if (!inst) return;
    if (Instance_IsA(inst, "PVInstance"))
    {
        //printf("draw %s.\n", inst->ClassName);
        PVInstance_Draw(inst);
    }
    for (int i = 0; i < inst->childCount; i++)
    {
        draw_recursive(inst->children[i]);
    }
}

void DataModel_Draw(DataModel *this)
{
    Camera_Instance *cam = this->Workspace->CurrentCamera;

    ClearBackground(SKYBLUE);

    BeginMode3D(cam->camera);
    Lighting *lighting = Instance_FindFirstChildOfClass(this, "Lighting");
    Lighting_draw(lighting);
    DrawCube((Vector3){0, 0, 0}, 1.0f, 1.0f, 1.0f, WHITE);
    draw_recursive(this->Workspace);
    DrawLine3D((Vector3){0, 0, 0}, (Vector3){100, 0, 0}, RED);
    DrawLine3D((Vector3){0, 0, 0}, (Vector3){0, 100, 0}, GREEN);
    DrawLine3D((Vector3){0, 0, 0}, (Vector3){0, 0, 100}, BLUE);

    // Part CFrame alignment tests.
    // Tags:
    // [NOSURFACE] there is a surface that hasn't been implemented yet.
    // [WORKS] everything is working properly with this model.
    // [NOCONSTRAINT] there is a constraint that hasn't been implemented yet.
    // [MISALIGN] something about the model is not aligned properly.
    // [NOBOTHER] won't bother with testing as the model is difficult to represent.

    // To test:
    // Uncomment the code that draws the model in magenta.
    // Load rbxmx_test with the corresponding model filename.
    // Check if they match up exactly and tweak the code in part.c accordingly.

    //TODO: Put these in a separate location.
    //TODO: Factor in model transformation whenever that gets implemented correctly.

    // 771 Block 2x4x1.rbxmx [NOSURFACE]
    //rlPushMatrix();
    //rlTranslatef(-17, 0, 6);
    //rlRotatef(90, 0, 1, 0);
    //DrawCube((Vector3){0, 0, 0}, 2, 1, 4, MAGENTA);
    //rlPopMatrix();

    // 772 Block 2x4x1 no join.rbxmx [WORKS]
    //DrawCube((Vector3){-13, 1, 8}, 2, 1, 4, MAGENTA);

    // 773 Block 2x4x1 hinged.rbxmx [NOCONSTRAINT]
    //DrawCube((Vector3){6, 0, 6}, 2, 1, 4, MAGENTA);

    // 774 Block 2x2x6.rbxmx [NOSURFACE]
    //DrawCube((Vector3){-12, 0, 4}, 2, 6, 2, MAGENTA);

    // 775 Block 4x8x1.rbxmx [NOSURFACE]
    //DrawCube((Vector3){-8, 0, 2}, 8, 1, 4, MAGENTA);

    // 776 Block 4x4x1 turntable.rbxmx [NOSURFACE] [NOCONSTRAINT]
    //DrawCube((Vector3){1, 0, 6}, 4, 1, 4, MAGENTA);

    // 777 Ball 2x2x2.rbxmx [WORKS]
    //DrawSphere((Vector3){-25 + 1, 0, 4 + 1}, 1, MAGENTA);

    // 780 Vertical Motor - Controllable.rbxmx [NOSURFACE] [NOCONSTRAINT]
    //DrawCube((Vector3){-24, 0, 14}, 4, 1, 4, MAGENTA);

    // 781 Motorized Block Pair - Controllable.rbxmx [NOSURFACE] [NOCONSTRAINT]
    //DrawCube((Vector3){-2, 1, -2}, 4, 1, 4, MAGENTA);
    //DrawCube((Vector3){-2, -1, 2}, 4, 1, 4, MAGENTA);

    // 782 Motorized Block Pair - Controllable.rbxmx [NOSURFACE] [NOCONSTRAINT]
    //DrawCube((Vector3){0, -1, -1}, 2, 2, 2, MAGENTA);
    //DrawCube((Vector3){-2, -1, -1}, 2, 2, 2, MAGENTA);

    // 783 Constant Velocity Motor Pair.rbxmx [NOSURFACE] [NOCONSTRAINT]
    //DrawCube((Vector3){-2, 1, -2}, 4, 1, 4, MAGENTA);
    //DrawCube((Vector3){-2, -1, 2}, 4, 1, 4, MAGENTA);

    // 784 Motorized Block - Controllable.rbxmx [NOSURFACE] [NOCONSTRAINT]
    //rlPushMatrix();
    //rlTranslatef(-38, 1, 3);
    //rlRotatef(90, 0, 1, 0);
    //DrawCube((Vector3){0, 0, 0}, 2, 2, 4, MAGENTA);
    //rlPopMatrix();

    // 788 Tree.rbxmx [MISALIGN] [NOSURFACE]
    //DrawCube((Vector3){-1, -9, 1}, 2, 8, 2, MAGENTA);
    //DrawSphere((Vector3){-5 + 5, -1.0f, -5 + 5}, 5, MAGENTA);

    // 789 Chassis.rbxmx [NOSURFACE] [NOCONSTRAINT] [NOBOTHER]
    // 794 Dog.rbxmx [MISALIGN] [NOSURFACE] [NOBOTHER]
    // 796 Skateboard.rbxmx [NOSURFACE] [NOCONSTRAINT] [NOBOTHER]
    // 797 Skooter.rbxmx [MISALIGN] [NOSURFACE] [NOBOTHER]
    // 801 Bouncy Chassis.rbxmx [MISALIGN] [NOSURFACE] [NOCONSTRAINT] [NOBOTHER]
    // 802 Micro Chassis.rbxmx

    // 843 25 Point flag.rbxmx [MISALIGN] [NOSURFACE]
    //DrawCube((Vector3){-1, -2, -1}, 1, 4, 1, MAGENTA);
    //DrawCube((Vector3){1, 0, -1}, 1, 2, 2, MAGENTA);

    EndMode3D();

    DrawFPS(0, 20);
    Camera_Process(cam);
}

DataModel *GetDataModel(void)
{
    return game;
}
