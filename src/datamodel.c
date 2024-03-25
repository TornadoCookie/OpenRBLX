#include "camera.h"
#include "datamodel.h"
#include <stddef.h>
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DEFAULT_DEBUG_CHANNEL(datamodel)

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
    if (Instance_IsA(inst, "PVInstance"))
    {
        printf("draw %s.\n", inst->ClassName);
        PVInstance_Draw(inst);
    }
    for (int i = 0; i < inst->childCount; i++)
    {
        draw_recursive(inst->children[i]);
    }
}

void DataModel_Draw(DataModel *this)
{
    Camera_Instance *cam;
    Instance **children;
    int childCount;

    children = Instance_GetChildren(this->Workspace, &childCount);

    for (int i = 0; i < childCount; i++)
    {
        if (!strcmp(children[i]->ClassName, "Camera"))
        {
            cam = children[i];
        }
    }


    ClearBackground(BLUE);
    DrawFPS(0, 0);

    BeginMode3D(cam->camera);
    DrawCube((Vector3){0, 0, 0}, 1.0f, 1.0f, 1.0f, WHITE);
    draw_recursive(this->Workspace);
    EndMode3D();

    Camera_Process(cam);
}
