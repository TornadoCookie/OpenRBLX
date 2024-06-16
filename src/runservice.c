#include "runservice.h"

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

    return newInst;
}

void RunService_Run(RunService *this)
{
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
