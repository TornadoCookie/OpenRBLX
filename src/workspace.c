#include <stdio.h>
#include <stdlib.h>
#include "camera.h"
#include "debug.h"
#include "workspace.h"
#include "datamodel.h"

DEFAULT_DEBUG_CHANNEL(workspace)

void workspace_draw(Workspace *this)
{

}

Workspace *Workspace_new(const char *className, Instance *parent)
{
    DataModel *datamodel = GetDataModel();
    Workspace *dmworkspace;
    if (dmworkspace = Instance_FindFirstChildOfClass(datamodel, "Workspace"))
    {
        return dmworkspace;
    }
    Workspace *newInst = RootInstance_new("Workspace", parent);

    newInst->rootinstance.model.pvinstance.instance.DataCost = sizeof(Workspace);
    newInst = realloc(newInst, sizeof(Workspace));

    Camera_Instance *camera = Camera_new(NULL, newInst);
    newInst->CurrentCamera = camera;

    newInst->rootinstance.model.pvinstance.drawFunc = workspace_draw;
    newInst->rootinstance.model.pvinstance.instance.Name = "Workspace";

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
