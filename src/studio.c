#include "studio.h"

Studio *Studio_new(const char *className, Instance *parent)
{
    Studio *newInst = Instance_new(className, parent);

    newInst = realloc(newInst, sizeof(*newInst));

    newInst->instance.Name = "Studio";

    newInst->ThemeChanged = RBXScriptSignal_new();

    if (parent)
        Instance_SetParent(newInst, parent);

    return newInst;
}

void serialize_Studio(Studio *studio, SerializeInstance *inst)
{
    serialize_Instance(studio, inst);

    serialize_atomic(event, studio, ThemeChanged);
}

