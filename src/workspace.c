#include "workspace.h"
#include "debug.h"
#include <stdio.h>

Workspace *Workspace_new(Instance *parent)
{
    Workspace *newInst = WorldRoot_new("Workspace", parent);

    newInst->worldroot.model.pvinstance.instance.DataCost = sizeof(Workspace);

    return newInst;
}

void Workspace_BreakJoints(Workspace *this, Instance **instances, int instanceCount)
{
    FIXME("this %p, instances %p, instanceCount %d stub!\n", this, instances, instanceCount);
}

void Workspace_MakeJoints(Workspace *this, Instance **instances, int instanceCount)
{
    FIXME("this %p, instances %p, instanceCount %d stub!\n", this, instances, instanceCount);
}

void Workspace_SetPhysicsThrottleEnabled(Workspace *this, bool value)
{
    FIXME("this %p, value %d stub!\n", this, value);
}

void Workspace_ZoomToExtents(Workspace *this)
{
    FIXME("this %p stub!\n", this);
}
