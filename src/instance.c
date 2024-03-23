#include "instance.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Instance *Instance_new(const char *className, Instance *parent)
{
    Instance *newInst = malloc(sizeof(Instance));

    newInst->archivable = true;
    newInst->ClassName = className;
    newInst->DataCost = sizeof(Instance);
    newInst->Name = "Instance";
    newInst->RobloxLocked = false;

    newInst->AncestryChanged = RBXScriptSignal_new();
    newInst->Changed = RBXScriptSignal_new();
    newInst->ChildAdded = RBXScriptSignal_new();
    newInst->ChildRemoved = RBXScriptSignal_new();
    newInst->DescendantAdded = RBXScriptSignal_new();
    newInst->DescendantRemoving = RBXScriptSignal_new();

    newInst->childCount = 0;
    newInst->children = NULL;

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

Instance *Instance_Clone(Instance *this)
{
    Instance *clone = malloc(sizeof(Instance));

    memcpy(clone, this, this->DataCost);

    return clone;
}

Instance *Instance_FindFirstChild(Instance *this, const char *name, bool recursive)
{
    for (int i = 0; i < this->childCount; i++)
    {
        if (!strcmp(this->children[i]->Name, name)) return this->children[i];
        if (recursive)
        {
            Instance *inst = Instance_FindFirstChild(this->children[i], name, recursive);
            if (inst) return inst;
        }
    }

    return NULL;
}

Instance **Instance_GetChildren(Instance *this, int *childCount)
{
    *childCount = this->childCount;
    return this->children;
}

const char *Instance_GetDebugId(Instance *this, int scopeLength)
{
    return this;
}

static void memcpy_reverse(unsigned char *restrict dst, const unsigned char *restrict src, size_t n)
{
    for (size_t i = 0; i < n; i++)
        dst[n-1-i] = src[i];
}

char *Instance_GetFullName(Instance *this)
{
    char *fullName = NULL;
    int fullNameLength = 0;
    int prevFullNameLength = 0;

    while (this->Parent != NULL)
    {
        bool hasDot = true;
        fullNameLength += strlen(this->Name);
        if (fullName != NULL)
        {
            fullNameLength++;
            hasDot = false;
        }
        fullName = realloc(fullName, fullNameLength);
        memcpy_reverse(fullName + fullNameLength - prevFullNameLength, fullName, prevFullNameLength);
        snprintf(fullName, strlen(this->Name) + 1, "%s%s", this->Name, hasDot ? "." : "");
        prevFullNameLength = fullNameLength;
        this = this->Parent;
    }

    return fullName;
}

bool Instance_IsA(Instance *this, const char *className)
{
    return false;
}

bool Instance_IsAncestorOf(Instance *this, Instance *descendant)
{
    for (int i = 0; i < this->childCount; i++)
    {
        if (this->children[i] == descendant) return true;
        if (Instance_IsAncestorOf(this->children[i], descendant)) return true;
    }
}

bool Instance_IsDescendantOf(Instance *this, Instance *ancestor)
{
    while (this != NULL)
    {
        if (this == ancestor) return true;
        this = this->Parent;
    }

    return false;
}

void Instance_Remove(Instance *this)
{
    if (this->Parent)
    {
        Instance *parent = this->Parent;
        while (parent != NULL)
        {
            RBXScriptSignal_Fire(this->Parent->DescendantRemoving, this);
            parent = parent->Parent;
        }

        for (int i = 0; i < this->Parent->childCount; i++)
        {
            if (this->Parent->children[i] == this)
            {
                this->Parent->children[i] = NULL;
            }
        }

        RBXScriptSignal_Fire(this->Parent->ChildRemoved, this);
    }
}

void Instance_SetArchivable(Instance *this, bool archivable) 
{
    this->archivable = archivable;
    RBXScriptSignal_Fire(this->Changed, "Archivable");
}

void Instance_SetName(Instance *this, const char *name)
{
    this->Name = name;
    RBXScriptSignal_Fire(this->Changed, "Name");
}

void Instance_SetParent(Instance *this, Instance *parent)
{
    EventArg_Instance_AncestryChanged *eventarg = malloc(sizeof(EventArg_Instance_AncestryChanged));
    eventarg->child = this;
    eventarg->parent = parent;

    this->Parent = parent;
    parent->childCount++;
    parent->children = realloc(parent->children, parent->childCount * sizeof(Instance*));
    parent->children[parent->childCount - 1] = this;
    
    RBXScriptSignal_Fire(this->Changed, "Parent");
    RBXScriptSignal_Fire(this->AncestryChanged, eventarg);
    RBXScriptSignal_Fire(parent->ChildAdded, this);
    
    //Instance *x = parent;
    //while (x != NULL)
    ///{
     //   RBXScriptSignal_Fire(x->DescendantAdded, this);
     //   x = x->Parent;
    //}
}
