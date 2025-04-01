#include "runservice.h"
#include "serverscriptservice.h"
#include "datamodel.h"
#include "players.h"
#include "testservice.h"

#include "debug.h"

DEFAULT_DEBUG_CHANNEL(runservice)

RunService *RunService_new(const char *className, Instance *parent)
{
    RunService *newInst = Instance_new("RunService", parent);

    newInst->instance.Name = "Run Service";
    newInst->instance.DataCost = sizeof(RunService);
    newInst = realloc(newInst, sizeof(RunService));

    newInst->Heartbeat = RBXScriptSignal_new();
    newInst->Stepped = RBXScriptSignal_new();
    newInst->running = false;

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

void RunService_Run(RunService *this)
{
    ServerScriptService *sss = ServiceProvider_GetService(GetDataModel(), "ServerScriptService");
    ServerScriptService_RunScripts(sss);

    Players *players = ServiceProvider_GetService(GetDataModel(), "Players");
    Player *localPlayer = players->LocalPlayer;
    if (localPlayer)
    {
        Player_RunScripts(localPlayer);
    }

    TestService *testService = ServiceProvider_GetService(GetDataModel(), "TestService");

    if (testService->AutoRuns)
    {
        TestService_Run(testService);
    }

    this->running = true;
}

void RunService_Reset(RunService *this)
{
    FIXME("this %p stub !", this);
    RunService_Pause(this);
}

void RunService_Pause(RunService *this)
{
    this->running = false;
}
