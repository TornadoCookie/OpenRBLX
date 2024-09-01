#include "instance.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <raylib.h>
#include "debug.h"
#include <dlfcn.h>
#include "serialize.h"
#include <time.h>

DEFAULT_DEBUG_CHANNEL(instance)

static int instanceCount;

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

    newInst->Parent = NULL;

    instanceCount++;

    return newInst;
}

int GetInstanceCount()
{
    return instanceCount;
}

Instance *Instance_Clone(Instance *this)
{
    Instance *clone = malloc(this->DataCost);

    memcpy(clone, this, this->DataCost);
    Instance_SetParent(clone, clone->Parent);

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
    char *buf = malloc(1024);
    snprintf(buf, 1023, "%p", this);
    buf = realloc(buf, strlen(buf) + 1);
    return buf;
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

Instance *Instance_WaitForChild(Instance *this, const char *childName, double timeOut)
{
    double elapsed = 0.0;
    bool saidInfiniteYield = false;
    while (timeOut == 0.0 || elapsed < timeOut)
    {
        clock_t st = clock();
        if (elapsed > 1 && !saidInfiniteYield)
        {
            const char *fullname = Instance_GetFullName(this);
            printf("Infinite yield possible on '%s:WaitForChild(\"%s\")'\n", fullname, childName);
            free(fullname);
            saidInfiniteYield = true;
        }
        Instance *try = Instance_FindFirstChild(this, childName, false);
        if (try) return try;
        clock_t ed = clock();
        double t = ((double)(ed - st)) / CLOCKS_PER_SEC;
        elapsed += t;
    }

    return NULL;
}

bool ClassName_IsA(const char *className1, const char *className)
{
    if (!strcmp(className, "Instance") || !strcmp(className1, className)) return true;

    else if (!strcmp(className, "PVInstance")) return ClassName_IsA(className1, "Model") || ClassName_IsA(className1, "BasePart");
    else if (!strcmp(className, "BasePart")) return ClassName_IsA(className1, "FormFactorPart") || ClassName_IsA(className1, "TrussPart") || ClassName_IsA(className1, "TriangleMeshPart");
    else if (!strcmp(className, "FormFactorPart")) return ClassName_IsA(className1, "Part");
    else if (!strcmp(className, "TriangleMeshPart")) return ClassName_IsA(className1, "MeshPart");

    else if (!strcmp(className, "Model")) return ClassName_IsA(className1, "WorldRoot");
    else if (!strcmp(className, "WorldRoot")) return ClassName_IsA(className1, "Workspace");

    else if (!strcmp(className, "LuaSourceContainer")) return ClassName_IsA(className1, "BaseScript");
    else if (!strcmp(className, "BaseScript")) return ClassName_IsA(className1, "Script");

    else if (!strcmp(className, "DataModelMesh")) return ClassName_IsA(className1, "FileMesh") || ClassName_IsA(className1, "BevelMesh");
    else if (!strcmp(className, "FileMesh")) return ClassName_IsA(className1, "SpecialMesh");
    else if (!strcmp(className, "BevelMesh")) return ClassName_IsA(className1, "BlockMesh") || ClassName_IsA(className1, "CylinderMesh");

    else if (!strcmp(className, "FaceInstance")) return ClassName_IsA(className1, "Decal");

    else if (!strcmp(className, "ServiceProvider")) return ClassName_IsA(className1, "DataModel");

    return false;
}

bool Instance_IsA(Instance *this, const char *className)
{
    return ClassName_IsA(this->ClassName, className);
}

bool Instance_IsAncestorOf(Instance *this, Instance *descendant)
{
    for (int i = 0; i < this->childCount; i++)
    {
        if (this->children[i] == descendant) return true;
        if (Instance_IsAncestorOf(this->children[i], descendant)) return true;
    }

    return false;
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

Instance *Instance_FindFirstChildWhichIsA(Instance *this, const char *className, bool recursive)
{
    for (int i = 0; i < this->childCount; i++)
    {
        if (Instance_IsA(this->children[i], className)) return this->children[i];
        else if (recursive)
        {
            Instance *ret = Instance_FindFirstChildWhichIsA(this->children[i], className, recursive);
            if (ret) return ret;
        }
    }

    return NULL;
}

Instance *Instance_FindFirstAncestorWhichIsA(Instance *this, const char *className)
{
    if (!this->Parent) return NULL;
    if (Instance_IsA(this->Parent, className)) return this->Parent;
    return Instance_FindFirstAncestorWhichIsA(this->Parent, className);
}

Instance **Instance_GetDescendants(Instance *this, int *childCount)
{
    Instance **ret = malloc(sizeof(Instance*) * this->childCount);
    int count = this->childCount;

    for (int i = 0; i < this->childCount; i++)
    {
        ret[i] = this->children[i];
        int oldCount = count;
        Instance **childDesc = Instance_GetDescendants(this->children[i], &count);
        ret = realloc(ret, sizeof(Instance*)*count);
        for (int j = oldCount; j < count; j++)
        {
            ret[j] = childDesc[j - oldCount];
        }
        free(childDesc);
    }

    *childCount += count;

    return ret;
}

Instance *Instance_FindFirstChildOfClass(Instance *this, const char *className)
{
    for (int i = 0; i < this->childCount; i++)
    {
        if (!this->children[i]) continue;
        if (!strcmp(this->children[i]->ClassName, className))
            return this->children[i];
    }

    return NULL;
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

    free(eventarg);
    
    //Instance *x = parent;
    //while (x != NULL)
    ///{
     //   RBXScriptSignal_Fire(x->DescendantAdded, this);
     //   x = x->Parent;
    //}
}

Instance *Instance_dynNew(const char *className, Instance *parent)
{
    //printf("Instance_dynNew(%s, %p)\n", className, parent);

    void *ourselves = dlopen(NULL, RTLD_LAZY);

    Instance *(*constructor)(const char *, Instance *);
    const char *constructorName = TextFormat("%s_new", className);
    constructor = dlsym(ourselves, constructorName);
    if (!constructor)
    {
        printf("dlerror: %s\n", dlerror());
    }
    dlclose(ourselves);

    if (!constructor)
    {
        return NULL;
    }

    return constructor(className, parent);
}

void Instance_Serialize(Instance *obj, SerializeInstance *inst)
{
    //printf("Instance_Serialize(%p, %p)\n", obj, inst);

    void *ourselves = dlopen(NULL, RTLD_LAZY);

    void (*serializer)(Instance*, SerializeInstance*);
    const char *serializerName = TextFormat("serialize_%s", obj->ClassName);
    serializer = dlsym(ourselves, serializerName);
    if (!serializer) printf("dlerror: %s\n", dlerror());
    dlclose(ourselves);

    if (!serializer)
    {
        printf("error: serializer for classname %s (serialize_%s() missing)\n", obj->ClassName, obj->ClassName);
        return;
    }
    serializer(obj, inst);
}

void serialize_Instance(Instance *instance, SerializeInstance *inst)
{
    serialize_atomic(string, instance, Name);
    serialize_atomic(bool, instance, archivable);
}
