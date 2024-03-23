#ifndef _WORKSPACE_
#define _WORKSPACE_

#include "rootinstance.h"

typedef struct Workspace {
    RootInstance rootinstance;

    double DistributedGameTime;
} Workspace;

Workspace *Workspace_new(Instance *parent);

void Workspace_BreakJoints(Workspace *this, Instance **instances, int instanceCount);
void Workspace_MakeJoints(Workspace *this, Instance **instances, int instanceCount);
void Workspace_SetPhysicsThrottleEnabled(Workspace *this, bool value);
void Workspace_ZoomToExtents(Workspace *this);

#endif
