#ifndef _INSTANCE_
#define _INSTANCE_

#include <stdbool.h>
#include "rbxscriptsignal.h"

typedef struct Instance Instance;

typedef struct Instance {
    // Properties
    bool archivable;
    const char *ClassName;
    unsigned int DataCost;
    const char *Name;
    Instance *Parent;
    bool RobloxLocked;

    // Events
    RBXScriptSignal *AncestryChanged;
    RBXScriptSignal *Changed;
    RBXScriptSignal *ChildAdded;
    RBXScriptSignal *ChildRemoved;
    RBXScriptSignal *DescendantAdded;
    RBXScriptSignal *DescendantRemoving;

    // Internal members
    Instance **children;
    int childCount;
    char *xmlref;
} Instance;

Instance *Instance_new(const char *className, Instance *parent);

Instance *Instance_Clone(Instance *this);
Instance *Instance_FindFirstChild(Instance *this, const char *name, bool recursive);
Instance **Instance_GetChildren(Instance *this, int *childCount);
const char *Instance_GetDebugId(Instance *this, int scopeLength);
char *Instance_GetFullName(Instance *this);
bool Instance_IsA(Instance *this, const char *className);
bool Instance_IsAncestorOf(Instance *this, Instance *descendant);
bool Instance_IsDescendantOf(Instance *this, Instance *ancestor);
void Instance_Remove(Instance *this);
Instance *Instance_FindFirstChildWhichIsA(Instance *this, const char *className, bool recursive);
Instance **Instance_GetDescendants(Instance *this, int *childCount);
Instance *Instance_FindFirstChildOfClass(Instance *this, const char *className);

bool ClassName_IsA(const char *className1, const char *className2);

void Instance_SetArchivable(Instance *this, bool archivable);
void Instance_SetName(Instance *this, const char *name);
void Instance_SetParent(Instance *this, Instance *parent);

typedef struct EventArg_Instance_AncestryChanged {
    Instance *child;
    Instance *parent;
} EventArg_Instance_AncestryChanged;

#endif
