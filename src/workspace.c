#include <stdio.h>
#include <stdlib.h>
#include "camera.h"
#include "debug.h"
#include "workspace.h"

void workspace_draw(Workspace *this)
{

}

Workspace *Workspace_new(Instance *parent)
{
    Workspace *newInst = RootInstance_new("Workspace", parent);

    newInst->rootinstance.model.pvinstance.instance.DataCost = sizeof(Workspace);
    newInst = realloc(newInst, sizeof(Workspace));

    Camera_Instance *camera = Camera_new(newInst);

    newInst->rootinstance.model.pvinstance.drawFunc = workspace_draw;

    if (parent) Instance_SetParent(newInst, parent);

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
