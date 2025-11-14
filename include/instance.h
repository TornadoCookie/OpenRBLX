#ifndef _INSTANCE_
#define _INSTANCE_

#include <stdbool.h>
#include "rbxscriptsignal.h"
#include "serialize.h"

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
    Serialization *attributes;
    int attributeCount;
    int propCount; 
    struct {
        RBXScriptSignal *event;
        const char *propName;
    } *propChangedSignals;
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
void Instance_Destroy(Instance *this);
Instance *Instance_FindFirstChildWhichIsA(Instance *this, const char *className, bool recursive);
Instance **Instance_GetDescendants(Instance *this, int *childCount);
Instance *Instance_FindFirstChildOfClass(Instance *this, const char *className);
Instance *Instance_FindFirstAncestorWhichIsA(Instance *this, const char *className);
Instance *Instance_WaitForChild(Instance *this, const char *childName, double timeOut);
Instance *Instance_FindFirstAncestor(Instance *this, const char *ancestorName);
void Instance_SetAttribute(Instance *this, Serialization sz);
Serialization Instance_GetAttribute(Instance *this, const char *name);
RBXScriptSignal *Instance_GetPropertyChangedSignal(Instance *this, const char *propName);

bool ClassName_IsA(const char *className1, const char *className2);
int GetInstanceCount(void);

void Instance_SetArchivable(Instance *this, bool archivable);
void Instance_SetName(Instance *this, const char *name);
void Instance_SetParent(Instance *this, Instance *parent);

Instance *Instance_dynNew(const char *className, Instance *parent);

void serialize_Instance(Instance *instance, SerializeInstance *inst);
void Instance_Serialize(Instance *obj, SerializeInstance *inst);

void Instance_FireChanged(Instance *this, const char *propName);

typedef struct EventArg_Instance_AncestryChanged {
    Instance *child;
    Instance *parent;
} EventArg_Instance_AncestryChanged;

typedef struct EventArg_Instance_Changed {
    Instance *inst;
    Serialization sz;
} EventArg_Instance_Changed;

#define INSTANCE_STUB_CONSTRUCTOR_BASE(T, B) \
    T *newInst = B##_new(className, parent); \
    ((Instance*)newInst)->DataCost = sizeof(*newInst); \
    newInst = realloc(newInst, sizeof(*newInst)); \
    return newInst;

#define INSTANCE_STUB_CONSTRUCTOR(T, B) \
    T *newInst = B##_new(className, parent); \
    ((Instance*)newInst)->DataCost = sizeof(*newInst); \
    newInst = realloc(newInst, sizeof(*newInst)); \
    if (parent && !strcmp(className, #T)) Instance_SetParent(newInst, parent); \
    return newInst;

#endif
