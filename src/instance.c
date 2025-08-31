#include <raylib.h>
#include "instance.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "debug.h"
#include "serialize.h"
#include <time.h>

DEFAULT_DEBUG_CHANNEL(instance);

#ifdef _WIN32
#include <libloaderapi.h>

static void *get_symbol_from_ourselves(const char *symbol)
{
    HMODULE ourselves = GetModuleHandleA(NULL);
    void *ret = GetProcAddress(ourselves, symbol);
    if (!ret) printf("GetProcAddress: %#x\n", GetLastError());

    // We do not use FreeLibrary() to free this handle because
    // that would cause us to unmap ourselves which would kill
    // us!!!

    return ret;
}

#else // Assume posix and therefore dlopen()
#include <dlfcn.h>

static void *get_symbol_from_ourselves(const char *symbol)
{
    void *ourselves = dlopen(NULL, RTLD_LAZY);
    void *ret = dlsym(ourselves, symbol);
    if (!ret) printf("dlerror: %s\n", dlerror());
    dlclose(ourselves);

    return ret;
}

#endif

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

    newInst->attributes = NULL;
    newInst->attributeCount = 0;

    newInst->propCount = 0;
    newInst->propChangedSignals = NULL;

    instanceCount++;

    return newInst;
}

int GetInstanceCount()
{
    return instanceCount;
}

Instance *Instance_Clone(Instance *this)
{
    if (!this->archivable) return NULL;

    Instance *clone = malloc(this->DataCost);
    memcpy(clone, this, this->DataCost);
    //Instance_SetParent(clone, clone->Parent);

    clone->childCount = 0;
    clone->children = NULL;

    for (int i = 0; i < this->childCount; i++)
    {
        Instance *childClone = Instance_Clone(this->children[i]);
        Instance_SetParent(childClone, clone);
    }

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
    if (!this)
    {
        return NULL;
    }

    char *parentFullName = Instance_GetFullName(this->Parent); 
    
    if (!parentFullName)
    {
        return strdup(this->Name);
    }

    int parentFullNameLen = strlen(parentFullName);
    char *fullName = realloc(parentFullName, parentFullNameLen + 1 + strlen(this->Name) + 1);

    fullName[parentFullNameLen] = '.';
    memcpy(fullName + parentFullNameLen + 1, this->Name, strlen(this->Name));

    fullName[parentFullNameLen + strlen(this->Name) + 1] = 0;

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
            char *fullname = Instance_GetFullName(this);
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

// TODO FIX THIS!!!!
bool ClassName_IsA(const char *className1, const char *className)
{
    if (!strcmp(className, "Instance") || !strcmp(className1, className)) return true;

    else if (!strcmp(className, "PVInstance")) return ClassName_IsA(className1, "Model") || ClassName_IsA(className1, "BasePart");
    else if (!strcmp(className, "BasePart")) return ClassName_IsA(className1, "FormFactorPart") || ClassName_IsA(className1, "TrussPart") || ClassName_IsA(className1, "TriangleMeshPart");
    else if (!strcmp(className, "FormFactorPart")) return ClassName_IsA(className1, "Part");
    else if (!strcmp(className, "TriangleMeshPart")) return ClassName_IsA(className1, "MeshPart");

    else if (!strcmp(className, "Model")) return ClassName_IsA(className1, "WorldRoot");
    else if (!strcmp(className, "WorldRoot")) return ClassName_IsA(className1, "Workspace");

    else if (!strcmp(className, "LuaSourceContainer")) return ClassName_IsA(className1, "BaseScript") || ClassName_IsA(className1, "ModuleScript");
    else if (!strcmp(className, "BaseScript")) return ClassName_IsA(className1, "Script") || ClassName_IsA(className1, "CoreScript");
    else if (!strcmp(className, "Script")) return ClassName_IsA(className1, "LocalScript");

    else if (!strcmp(className, "DataModelMesh")) return ClassName_IsA(className1, "FileMesh") || ClassName_IsA(className1, "BevelMesh");
    else if (!strcmp(className, "FileMesh")) return ClassName_IsA(className1, "SpecialMesh");
    else if (!strcmp(className, "BevelMesh")) return ClassName_IsA(className1, "BlockMesh") || ClassName_IsA(className1, "CylinderMesh");

    else if (!strcmp(className, "FaceInstance")) return ClassName_IsA(className1, "Decal");

    else if (!strcmp(className, "ServiceProvider")) return ClassName_IsA(className1, "DataModel") || ClassName_IsA(className1, "GenericSettings");
    else if (!strcmp(className, "Part")) return ClassName_IsA(className1, "SpawnLocation");
    else if (!strcmp(className, "GenericSettings")) return ClassName_IsA(className1, "GlobalSettings");
    else if (!strcmp(className, "StyleBase")) return ClassName_IsA(className1, "StyleRule") || ClassName_IsA(className1, "StyleSheet");
    //else FIXME("className1 %s, className %s\n", className1, className);

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

void Instance_Destroy(Instance *this)
{
    Instance_Remove(this);
    free(this);
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

Instance *Instance_FindFirstAncestor(Instance *this, const char *name)
{
    if (!this->Parent) return NULL;
    if (!strcmp(this->Parent->Name, name)) return this->Parent;
    return Instance_FindFirstAncestor(this->Parent, name);
}

void Instance_SetAttribute(Instance *this, Serialization sz)
{
    for (int i = 0; i < this->attributeCount; i++)
    {
        if (!strcmp(this->attributes[i].name, sz.name))
        {
            free(this->attributes[i].val);
            this->attributes[i] = sz;
            return;
        }
    }

    this->attributeCount++;
    this->attributes = realloc(this->attributes, sizeof(Serialization) * this->attributeCount);
    this->attributes[this->attributeCount - 1] = sz;
}

Serialization Instance_GetAttribute(Instance *this, const char *name)
{
    for (int i = 0; i < this->attributeCount; i++)
    {
        if (!strcmp(this->attributes[i].name, name))
        {
            return this->attributes[i];
        }
    }

    return (Serialization){.name = NULL, .val = NULL};
}

static void get_property_changed_signal_dispatcher(void *arg)
{
    EventArg_Instance_Changed *changed = arg;

    for (int i = 0; i < changed->inst->propCount; i++)
    {
        if (!strcmp(changed->sz.name, changed->inst->propChangedSignals[i].propName))
        {
            RBXScriptSignal_Fire(changed->inst->propChangedSignals[i].event, NULL);
        }
    }
}

RBXScriptSignal *Instance_GetPropertyChangedSignal(Instance *this, const char *propName)
{
    if (!this->propChangedSignals)
    {
        SerializeInstance sInst = { 0 };
        Instance_Serialize(this, &sInst);

        this->propCount = sInst.serializationCount;
        this->propChangedSignals = malloc(sizeof(*this->propChangedSignals) * sInst.serializationCount);

        for (int i = 0; i < sInst.serializationCount; i++)
        {
            this->propChangedSignals[i].event = RBXScriptSignal_new();
            this->propChangedSignals[i].propName = sInst.serializations[i].name;
        }

        RBXScriptSignal_Connect(this->Changed, get_property_changed_signal_dispatcher, NULL);
    }

    for (int i = 0; i < this->propCount; i++)
    {
        if (!strcmp(this->propChangedSignals[i].propName, propName))
        {
            return this->propChangedSignals[i].event;
        }
    }

    ERR("no prop %s for %s\n", propName, this->ClassName);
    
    return NULL;
}

void Instance_SetArchivable(Instance *this, bool archivable) 
{
    this->archivable = archivable;
    Instance_FireChanged(this, "Archivable");
}

void Instance_SetName(Instance *this, const char *name)
{
    this->Name = name;
    Instance_FireChanged(this, "Name");
}

void Instance_SetParent(Instance *this, Instance *parent)
{
    EventArg_Instance_AncestryChanged *eventarg = malloc(sizeof(EventArg_Instance_AncestryChanged));
    eventarg->child = this;
    eventarg->parent = parent;

    this->Parent = parent;

    if (parent)
    {
        parent->childCount++;
        parent->children = realloc(parent->children, parent->childCount * sizeof(Instance*));
        parent->children[parent->childCount - 1] = this;
    }

    //Instance_FireChanged(this, "Parent");
    RBXScriptSignal_Fire(this->AncestryChanged, eventarg);
    if (parent)
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

    Instance *(*constructor)(const char *, Instance *);
    const char *constructorName = TextFormat("%s_new", className);
    constructor = get_symbol_from_ourselves(constructorName);

    if (!constructor)
    {
        return NULL;
    }

    return constructor(className, parent);
}

void Instance_Serialize(Instance *obj, SerializeInstance *inst)
{
    //printf("Instance_Serialize(%p, %p)\n", obj, inst);

    void (*serializer)(Instance*, SerializeInstance*);
    const char *serializerName = TextFormat("serialize_%s", obj->ClassName);
    serializer = get_symbol_from_ourselves(serializerName);

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

void Instance_FireChanged(Instance *this, const char *propName)
{
    SerializeInstance sInst = { 0 };

    Instance_Serialize(this, &sInst);

    EventArg_Instance_Changed *evtarg;

    evtarg = malloc(sizeof(*evtarg));
    evtarg->inst = this;

    for (int i = 0; i < sInst.serializationCount; i++)
    {
        if (!strcmp(sInst.serializations[i].name, propName))
        {
            evtarg->sz = sInst.serializations[i];
        }
    }

    free(sInst.serializations);
}

